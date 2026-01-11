// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenu/KRSlotNameWidget.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"

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

	EnsureStringTablesLoaded();

	if (PrimaryButton)
	{
		PrimaryButton->OnClicked().AddUObject(this, &ThisClass::HandlePrimaryClicked);
	}
	if (SecondaryButton)
	{
		SecondaryButton->OnClicked().AddUObject(this, &ThisClass::HandleSecondaryClicked);
	}
}

void UKRSlotNameWidget::EnsureStringTablesLoaded()
{
	if (bStringTablesLoaded)
	{
		return;
	}

	if (!UIStringTableAsset.IsNull())
	{
		CachedUIStringTable = UIStringTableAsset.LoadSynchronous();
	}

	if (!ItemStringTableAsset.IsNull())
	{
		CachedItemStringTable = ItemStringTableAsset.LoadSynchronous();
	}

	bStringTablesLoaded = true;
}

FText UKRSlotNameWidget::MakeText(FName Key)
{
	if (Key.IsNone())
	{
		return FText::GetEmpty();
	}

	EnsureStringTablesLoaded();

	if (CachedUIStringTable)
	{
		FText Result = FText::FromStringTable(CachedUIStringTable->GetStringTableId(), Key.ToString());
		if (!Result.IsEmpty())
		{
			return Result;
		}
	}

	return FText::FromName(Key);
}

FText UKRSlotNameWidget::MakeItemName(FName Key)
{
	if (Key.IsNone())
	{
		return FText::GetEmpty();
	}

	EnsureStringTablesLoaded();

	if (CachedItemStringTable)
	{
		FText Result = FText::FromStringTable(CachedItemStringTable->GetStringTableId(), Key.ToString());
		if (!Result.IsEmpty())
		{
			return Result;
		}
	}

	return FText::FromName(Key);
}

void UKRSlotNameWidget::HandlePrimaryClicked()
{
	OnPrimaryClicked.Broadcast(Context);
}

void UKRSlotNameWidget::HandleSecondaryClicked()
{
	OnSecondaryClicked.Broadcast(Context);
}
