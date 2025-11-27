// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Modal/KRConfirmModal.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"

void UKRConfirmModal::SetupConfirm(const FText& InMessage, EConfirmContext InContext, FGameplayTag InItemTag)
{
    if (AlertText)
    {
        AlertText->SetText(InMessage);
    }

    Context = InContext;
    ItemTag = InItemTag;
    bUseQuantity = false;
    CurrentQuantity = 0;

    if (QuantityPanel)
    {
        QuantityPanel->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UKRConfirmModal::SetupConfirmWithQuantity(const FText& InMessage, EConfirmContext InContext, FGameplayTag InItemTag, int32 InMin, int32 InMax, int32 InDefault)
{
    if (AlertText)
    {
        AlertText->SetText(InMessage);
    }

    Context = InContext;
    ItemTag = InItemTag;
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

void UKRConfirmModal::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (YesButton) YesButton->OnClicked().AddUObject(this, &ThisClass::HandleYes);
    if (NoButton) NoButton->OnClicked().AddUObject(this, &ThisClass::HandleNo);
    if (IncreaseButton) IncreaseButton->OnClicked().AddUObject(this, &ThisClass::HandleIncrease);
    if (DecreaseButton) DecreaseButton->OnClicked().AddUObject(this, &ThisClass::HandleDecrease);
}

void UKRConfirmModal::NativeOnDeactivated()
{
    Super::NativeOnDeactivated();
}

void UKRConfirmModal::HandleYes()
{
    if (UWorld* World = GetWorld())
    {
        FKRUIMessage_Confirm Msg;
        Msg.Context = Context;
        Msg.Result = EConfirmResult::Yes;
        Msg.ItemTag = ItemTag;
        Msg.Quantity = bUseQuantity ? CurrentQuantity : 0;

        UGameplayMessageSubsystem::Get(World).BroadcastMessage(
            FKRUIMessageTags::Confirm(),
            Msg
        );
    }

    DeactivateWidget();
}

void UKRConfirmModal::HandleNo()
{
    DeactivateWidget();
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

