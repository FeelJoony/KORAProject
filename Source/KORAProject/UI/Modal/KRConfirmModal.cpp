#include "UI/Modal/KRConfirmModal.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "Groups/CommonButtonGroupBase.h"

void UKRConfirmModal::SetupConfirm(FName MessageKey, EConfirmContext InContext, FGameplayTag InItemTag)
{
    UpdateAlertText(MessageKey);
    Context = InContext;
    ItemTag = InItemTag;
	QuickSlotTag = FGameplayTag();
    bUseQuantity = false;
    CurrentQuantity = 0;

    if (QuantityPanel)
    {
        QuantityPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UKRConfirmModal::SetupConfirmWithQuantity(FName MessageKey, EConfirmContext InContext, FGameplayTag InItemTag, int32 InMin, int32 InMax, int32 InDefault)
{
    UpdateAlertText(MessageKey);
    Context = InContext;
    ItemTag = InItemTag;
	QuickSlotTag = FGameplayTag();
    bUseQuantity = true;

    MinQuantity = InMin;
    MaxQuantity = InMax;
    CurrentQuantity = FMath::Clamp(InDefault, MinQuantity, MaxQuantity);

    if (QuantityPanel)
    {
        QuantityPanel->SetVisibility(ESlateVisibility::Visible);
    }
    RefreshQuantityUI();
}

void UKRConfirmModal::SetupConfirmWithQuickSlotTag(FName MessageKey, EConfirmContext InContext, FGameplayTag InItemTag, FGameplayTag InSlotTag)
{
    UpdateAlertText(MessageKey);
	Context = InContext;
	ItemTag = InItemTag;
	QuickSlotTag = InSlotTag;
	bUseQuantity = false;
	CurrentQuantity = 0;

	if (QuantityPanel)
	{
		QuantityPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRConfirmModal::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    ConfirmButtonGroup = NewObject<UCommonButtonGroupBase>(this);
    ConfirmButtonGroup->SetSelectionRequired(true);
    
    InitializeConfirmButtons();

    if (YesButton)
    {
        YesButton->OnClicked().AddUObject(this, &ThisClass::HandleYes);
        YesButton->OnHovered().AddLambda([this]()
        {
            int32 HoveredIndex = ConfirmButtons.IndexOfByKey(YesButton);
            if (HoveredIndex != INDEX_NONE)
            {
                UpdateButtonSelection(HoveredIndex);
            }
        });
    }

    if (NoButton)
    {
        NoButton->OnClicked().AddUObject(this, &ThisClass::HandleNo);
        NoButton->OnHovered().AddLambda([this]()
        {
            int32 HoveredIndex = ConfirmButtons.IndexOfByKey(NoButton);
            if (HoveredIndex != INDEX_NONE)
            {
                UpdateButtonSelection(HoveredIndex);
            }
        });
    }

    if (IncreaseButton) IncreaseButton->OnClicked().AddUObject(this, &ThisClass::HandleIncrease);
    if (DecreaseButton) DecreaseButton->OnClicked().AddUObject(this, &ThisClass::HandleDecrease);
}

void UKRConfirmModal::NativeOnActivated()
{
    Super::NativeOnActivated();
    
    if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
    {
        InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMovePrev));
        InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveNext));
        InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
    }
    
    UpdateButtonSelection(0);
}

void UKRConfirmModal::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();
    
    ULocalPlayer* LP = GetOwningLocalPlayer();
    if (!LP)
    {
        if (APlayerController* PC = GetOwningPlayer())
        {
            LP = PC->GetLocalPlayer();
        }
    }

    if (!LP)
    {
        return;
    }

    if (UKRUIInputSubsystem* InputSubsys = LP->GetSubsystem<UKRUIInputSubsystem>())
    {
        InputSubsys->UnbindAll(this);
    }
}

void UKRConfirmModal::HandleYes()
{
    if (UWorld* World = GetWorld())
    {
        FKRUIMessage_Confirm Msg;
        Msg.Context = Context;
        Msg.Result = EConfirmResult::Yes;
        Msg.ItemTag = ItemTag;
		Msg.SlotTag = QuickSlotTag;
        Msg.Quantity = bUseQuantity ? CurrentQuantity : 0;

        UGameplayMessageSubsystem::Get(World).BroadcastMessage(
            FKRUIMessageTags::Confirm(),
            Msg
        );
    }

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
        {
            Router->ToggleRoute(TEXT("Confirm"));
        }
    }
}

void UKRConfirmModal::HandleNo()
{
    if (UWorld* World = GetWorld())
    {
        FKRUIMessage_Confirm Msg;
        Msg.Context = Context;
        Msg.Result = EConfirmResult::No;
        Msg.ItemTag = ItemTag;
        Msg.SlotTag = QuickSlotTag;
        Msg.Quantity = 0;

        UGameplayMessageSubsystem::Get(World).BroadcastMessage(
            FKRUIMessageTags::Confirm(),
            Msg
        );
    }
    
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
        {
            Router->ToggleRoute(TEXT("Confirm"));
        }
    }
}

void UKRConfirmModal::HandleIncrease()
{
    if (!bUseQuantity) return;

    CurrentQuantity = FMath::Clamp(CurrentQuantity + 1, MinQuantity, MaxQuantity);
    RefreshQuantityUI();
}

void UKRConfirmModal::HandleDecrease()
{
    if (!bUseQuantity) return;

    CurrentQuantity = FMath::Clamp(CurrentQuantity - 1, MinQuantity, MaxQuantity);
    RefreshQuantityUI();
}

void UKRConfirmModal::RefreshQuantityUI()
{
    if (!QuantityText) return;

    QuantityText->SetText(FText::AsNumber(CurrentQuantity));

    if (IncreaseButton)
    {
        IncreaseButton->SetIsEnabled(CurrentQuantity < MaxQuantity);
    }
    if (DecreaseButton)
    {
        DecreaseButton->SetIsEnabled(CurrentQuantity > MinQuantity);
    }
}

void UKRConfirmModal::UpdateAlertText(FName MessageKey)
{
    FText Msg = FText::FromStringTable(
        TEXT("/Game/UI/StringTable/ST_UIBaseTexts"),
        MessageKey.ToString()
    );

    if (AlertText)
    {
        AlertText->SetText(Msg);
    }
}

void UKRConfirmModal::InitializeConfirmButtons()
{
    ConfirmButtons.Empty();
    
    if (YesButton)
    {
        ConfirmButtons.Add(YesButton);
        ConfirmButtonGroup->AddWidget(YesButton);
    }

    if (NoButton)
    {
        ConfirmButtons.Add(NoButton);
        ConfirmButtonGroup->AddWidget(NoButton);
    }
}

void UKRConfirmModal::UpdateButtonSelection(int32 NewIndex)
{
    if (!ConfirmButtons.IsValidIndex(NewIndex))
    {
        return;
    }

    CurrentButtonIndex = NewIndex;
    
    if (ConfirmButtonGroup)
    {
        ConfirmButtonGroup->SelectButtonAtIndex(NewIndex);
    }
    
    if (UCommonButtonBase* SelectedButton = ConfirmButtons[NewIndex])
    {
        SelectedButton->SetFocus();
    }
}

void UKRConfirmModal::HandleMovePrev()
{
    int32 NewIndex = FMath::Max(0, CurrentButtonIndex - 1);
    if (NewIndex != CurrentButtonIndex)
    {
        UpdateButtonSelection(NewIndex);
    }
}

void UKRConfirmModal::HandleMoveNext()
{
    int32 NewIndex = FMath::Min(ConfirmButtons.Num() - 1, CurrentButtonIndex + 1);
    if (NewIndex != CurrentButtonIndex)
    {
        UpdateButtonSelection(NewIndex);
    }
}

void UKRConfirmModal::HandleSelect()
{
    if (!ConfirmButtons.IsValidIndex(CurrentButtonIndex))
    {
        return;
    }

    if (UCommonButtonBase* SelectedButton = ConfirmButtons[CurrentButtonIndex])
    {
        SelectedButton->OnClicked().Broadcast();
    }
}
