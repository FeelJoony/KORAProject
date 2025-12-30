// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenu/KRSlotNameWidget.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "Internationalization/StringTableRegistry.h"

void UKRSlotNameWidget::SetupForMenu(FName MenuNameKey)
{
	Context = EKRSlotNameContext::Menu;

	if (LabelText) LabelText->SetText(MakeText(MenuNameKey));

	if (PrimaryButton)
	{
		PrimaryButton->SetIsEnabled(true);
		BP_SetButtonText(PrimaryButton, MakeText(TEXT("Btn_Select")));
	}

	if (SecondaryButton)
	{
		SecondaryButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRSlotNameWidget::SetupForQuickSlot(FName ItemNameKey)
{
	if (LabelText)
	{
		if (ItemNameKey.IsNone()) 
		{
			LabelText->SetText(MakeText(TEXT("Slot_Empty")));

			Context = EKRSlotNameContext::QuickSlot_Empty;

			if (PrimaryButton)
			{
				PrimaryButton->SetVisibility(ESlateVisibility::Visible);
				BP_SetButtonText(PrimaryButton, MakeText(TEXT("Btn_Apply")));
			}
			if (SecondaryButton)
			{
				SecondaryButton->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			LabelText->SetText(MakeItemName(ItemNameKey));

			Context = EKRSlotNameContext::QuickSlot_HasItem;

			if (PrimaryButton)
			{
				PrimaryButton->SetVisibility(ESlateVisibility::Visible);
				BP_SetButtonText(PrimaryButton, MakeText(TEXT("Btn_Switch")));
			}
			if (SecondaryButton)
			{
				SecondaryButton->SetVisibility(ESlateVisibility::Visible);
				BP_SetButtonText(SecondaryButton, MakeText(TEXT("Btn_Remove")));
			}
		}
	}
}

void UKRSlotNameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PrimaryButton)
	{
		PrimaryButton->OnClicked().AddUObject(this, &ThisClass::HandlePrimaryClicked);
	}
	if (SecondaryButton)
	{
		SecondaryButton->OnClicked().AddUObject(this, &ThisClass::HandleSecondaryClicked);
	}
}

FText UKRSlotNameWidget::MakeText(FName Key) const
{
	return FText::FromStringTable(StringTableId, Key.ToString());
}

FText UKRSlotNameWidget::MakeItemName(FName Key) const
{
	return FText::FromStringTable(ItemTableId, Key.ToString());
}

void UKRSlotNameWidget::HandlePrimaryClicked()
{
	OnPrimaryClicked.Broadcast(Context);
}

void UKRSlotNameWidget::HandleSecondaryClicked()
{
	OnSecondaryClicked.Broadcast(Context);
}
