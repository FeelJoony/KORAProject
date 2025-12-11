// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tutorial/KRTutorialHintWidget.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "CommonTextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"

static const TCHAR* ST_Tutorial_Path = TEXT("/Game/UI/StringTable/ST_Tutorial");

void UKRTutorialHintWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    SetVisibility(ESlateVisibility::Collapsed);

    if (UWorld* World = GetWorld())
    {
        UGameplayMessageSubsystem& Subsystem = UGameplayMessageSubsystem::Get(World);

        TutorialListener = Subsystem.RegisterListener<FKRUIMessage_Tutorial>(
            FKRUIMessageTags::Tutorial(),
            this,
            &UKRTutorialHintWidget::HandleTutorialMessage
        );
    }

    bHasActiveRow = false;
}

void UKRTutorialHintWidget::NativeDestruct()
{
    if (TutorialListener.IsValid() && GetWorld())
    {
        UGameplayMessageSubsystem::Get(GetWorld()).UnregisterListener(TutorialListener);
    }

    ClearTimerAndUnpause();
    bHasActiveRow = false;

    Super::NativeDestruct();
}

void UKRTutorialHintWidget::NativeOnActivated()
{
    Super::NativeOnActivated();

    if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
    {
		InputSubsys->BindRow(this, TEXT("Back"), FSimpleDelegate::CreateUObject(this, &UKRTutorialHintWidget::HandleOk));
        InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &UKRTutorialHintWidget::HandleOk));
    }
}

void UKRTutorialHintWidget::NativeOnDeactivated()
{

    Super::NativeOnDeactivated();
}


void UKRTutorialHintWidget::HandleTutorialMessage(FGameplayTag Channel, const FKRUIMessage_Tutorial& Msg)
{
    if (!TutorialPopupTable || Msg.TutorialDTRowName.IsNone()) return;
    const FTutorialDataStruct* Row = TutorialPopupTable->FindRow<FTutorialDataStruct>(Msg.TutorialDTRowName, TEXT("KRTutorialHintWidget::HandleTutorialMessage"));

    if (!Row) return;
    ApplyDataTableRow(*Row);
}

void UKRTutorialHintWidget::ApplyDataTableRow(const FTutorialDataStruct& Row)
{
    ClearTimerAndUnpause();

    CurrentRow = Row;
    bHasActiveRow = true;

    if (TutorialText)
    {
        FText LocalizedText = FText::FromStringTable(ST_Tutorial_Path, Row.StringKey.ToString());
        TutorialText->SetText(LocalizedText);
    }

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetPosition(FVector2D(Row.OffsetX, Row.OffsetY));
    }

    SetVisibility(ESlateVisibility::HitTestInvisible);

    if (Row.PauseGame && GetWorld())
    {
        UGameplayStatics::SetGamePaused(GetWorld(), true);
        bPausedByMe = true;
    }

    if (IsTimedPopup(Row) && Row.Duration > 0 && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            DurationHandle,
            this,
            &UKRTutorialHintWidget::OnDurationTimeout,
            Row.Duration,
            false
        );
    }

    BP_OnTutorialShown(); // For Animation
    if (Row.PopupType == TEXT("WaitInput") || Row.PopupType == TEXT("TimedOrInput"))
    {
        BP_ShowPressSpace();
    }
}

void UKRTutorialHintWidget::ClosePopup()
{
    if (!bHasActiveRow)
        return;

    ClearTimerAndUnpause();
    bHasActiveRow = false;

    SetVisibility(ESlateVisibility::Collapsed);
}

void UKRTutorialHintWidget::OnDurationTimeout()
{
    if (bHasActiveRow && IsTimedPopup(CurrentRow))
    {
        ClosePopup();
    }
}

bool UKRTutorialHintWidget::IsTimedPopup(const FTutorialDataStruct& Row) const
{
    return Row.PopupType.Equals(TEXT("Timed"), ESearchCase::IgnoreCase) ||
        Row.PopupType.Equals(TEXT("TimedOrInput"), ESearchCase::IgnoreCase);
}

bool UKRTutorialHintWidget::IsInputPopup(const FTutorialDataStruct& Row) const
{
    return Row.PopupType.Equals(TEXT("WaitInput"), ESearchCase::IgnoreCase) ||
        Row.PopupType.Equals(TEXT("TimedOrInput"), ESearchCase::IgnoreCase);
}

void UKRTutorialHintWidget::ClearTimerAndUnpause()
{
    if (!GetWorld()) return;

    GetWorld()->GetTimerManager().ClearTimer(DurationHandle);

    if (bPausedByMe)
    {
        UGameplayStatics::SetGamePaused(GetWorld(), false);
        bPausedByMe = false;
    }
}

void UKRTutorialHintWidget::HandleOk()
{
	if (bHasActiveRow && IsInputPopup(CurrentRow))
	{
		ClosePopup();
        BP_HidePressSpace();
	}

    if (APlayerController* PC = GetOwningLocalPlayer()->GetPlayerController(GetWorld()))
    {
        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
        PC->SetShowMouseCursor(false);
    }
}