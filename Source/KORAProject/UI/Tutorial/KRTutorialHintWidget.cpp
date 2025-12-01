// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tutorial/KRTutorialHintWidget.h"

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

void UKRTutorialHintWidget::NotifyStepCompleted(FName StepId)
{
    if (!bHasActiveRow || CurrentRow.CompletionTrigger.IsNone())
        return;

    if (StepId == CurrentRow.CompletionTrigger && IsInputPopup(CurrentRow))
    {
        ClosePopup();
    }

}

void UKRTutorialHintWidget::HandleTutorialMessage(FGameplayTag Channel, const FKRUIMessage_Tutorial& Msg)
{
    if (!TutorialPopupTable || Msg.TutorialDTRowName.IsNone()) return;
    const FTutorialDataStruct* Row = TutorialPopupTable->FindRow<FTutorialDataStruct>(Msg.TutorialDTRowName, TEXT("KRTutorialHintWidget::HandleTutorialMessage"));

    if (!Row) return;
    ApplyRow(*Row);
}

void UKRTutorialHintWidget::ApplyRow(const FTutorialDataStruct& Row)
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

    ActivateWidget();
    BP_OnTutorialShown();
}

void UKRTutorialHintWidget::ClosePopup()
{
    if (!bHasActiveRow)
        return;

    ClearTimerAndUnpause();
    bHasActiveRow = false;

    DeactivateWidget();
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
