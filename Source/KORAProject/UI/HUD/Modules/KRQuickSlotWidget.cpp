// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/Modules/KRQuickSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Engine/World.h"

void UKRQuickSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		QuickSlotListener = Subsys.RegisterListener(
			FKRUIMessageTags::QuickSlot(),
			this,
			&ThisClass::OnQuickSlotMessageReceived
		);
	}

	ClearSlot(EQuickSlotDirection::North);
	ClearSlot(EQuickSlotDirection::East);
	ClearSlot(EQuickSlotDirection::South);
	ClearSlot(EQuickSlotDirection::West);

	// Initialize Quickslot 

	HighlightSlot(CurrentSelectedSlot);
}

void UKRQuickSlotWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(QuickSlotListener);
	}

	Super::NativeDestruct();
}

void UKRQuickSlotWidget::OnQuickSlotMessageReceived(FGameplayTag Channel, const FKRUIMessage_QuickSlot& Message)
{
	const EQuickSlotDirection SlotDir = Message.CurrentlySelectedSlot;

	switch (Message.ActionType)
	{
	case EQuickSlotAction::ItemRegistered:
	{
		UpdateSlot(SlotDir, Message.ItemQuantity, Message.ItemIconPath);
		CurrentSelectedSlot = SlotDir;
		HighlightSlot(CurrentSelectedSlot);
		BP_OnSlotRegistered(SlotDir);
		BP_OnSlotSelectionChanged(CurrentSelectedSlot);
		break;
	}

	case EQuickSlotAction::ItemUnregistered:
	{
		ClearSlot(SlotDir);
		break;
	}

	case EQuickSlotAction::ItemUsed:
	{
		BP_OnItemUsedFromSlot(SlotDir, Message.Duration);
		break;
	}

	case EQuickSlotAction::SlotChanged:
	{
		CurrentSelectedSlot = SlotDir;
		HighlightSlot(CurrentSelectedSlot);
		BP_OnSlotSelectionChanged(CurrentSelectedSlot);
		break;
	}

	case EQuickSlotAction::QuantityUpdated:
	{
		UpdateSlotQuantity(SlotDir, Message.ItemQuantity);
		break;
	}
	}
}

void UKRQuickSlotWidget::UpdateSlot(EQuickSlotDirection Direction, int32 Quantity, const FSoftObjectPath& IconPath)
{
	UImage* Icon = nullptr;
	UTextBlock* QuantityText = nullptr;
	UBorder* Border = nullptr;
	GetSlotWidgets(Direction, Icon, QuantityText, Border);

	if (Icon)
	{
		if (IconPath.IsValid())
		{
			Icon->SetOpacity(1.0f);
		}
		else
		{
			Icon->SetOpacity(EmptySlotOpacity);
		}
	}

	if (QuantityText)
	{
		if (Quantity > 0)
		{
			QuantityText->SetText(FText::AsNumber(Quantity));
		}
		else
		{
			QuantityText->SetText(FText::GetEmpty());
		}
	}

	if (Border)
	{
		Border->SetBrushColor(NormalColor);
	}
}

void UKRQuickSlotWidget::ClearSlot(EQuickSlotDirection Direction)
{
	UImage* Icon = nullptr;
	UTextBlock* QuantityText = nullptr;
	UBorder* Border = nullptr;
	GetSlotWidgets(Direction, Icon, QuantityText, Border);

	if (Icon)
	{
		Icon->SetOpacity(EmptySlotOpacity);
	}

	if (QuantityText)
	{
		QuantityText->SetText(FText::GetEmpty());
	}

	if (Border)
	{
		Border->SetBrushColor(NormalColor);
	}
}

void UKRQuickSlotWidget::HighlightSlot(EQuickSlotDirection Direction)
{
	auto ResetBorder = [this](EQuickSlotDirection Dir)
		{
			UImage* DummyIcon = nullptr;
			UTextBlock* DummyText = nullptr;
			UBorder* Border = nullptr;
			GetSlotWidgets(Dir, DummyIcon, DummyText, Border);
			if (Border)
			{
				Border->SetBrushColor(NormalColor);
			}
		};

	ResetBorder(EQuickSlotDirection::North);
	ResetBorder(EQuickSlotDirection::East);
	ResetBorder(EQuickSlotDirection::South);
	ResetBorder(EQuickSlotDirection::West);

	UImage* Icon = nullptr;
	UTextBlock* Text = nullptr;
	UBorder* Border = nullptr;
	GetSlotWidgets(Direction, Icon, Text, Border);

	if (Border)
	{
		Border->SetBrushColor(HighlightColor);
	}
}

void UKRQuickSlotWidget::UpdateSlotQuantity(EQuickSlotDirection Direction, int32 NewQuantity)
{
	UImage* Icon = nullptr;
	UTextBlock* QuantityText = nullptr;
	UBorder* Border = nullptr;
	GetSlotWidgets(Direction, Icon, QuantityText, Border);

	if (QuantityText)
	{
		if (NewQuantity > 0)
		{
			QuantityText->SetText(FText::AsNumber(NewQuantity));
		}
		else
		{
			QuantityText->SetText(FText::GetEmpty());
		}
	}
}

void UKRQuickSlotWidget::GetSlotWidgets(EQuickSlotDirection Direction, UImage*& OutIcon, UTextBlock*& OutQuantity, UBorder*& OutBorder)
{
	OutIcon = nullptr;
	OutQuantity = nullptr;
	OutBorder = nullptr;

	switch (Direction)
	{
	case EQuickSlotDirection::North:
		OutIcon = NorthSlotIcon;
		OutQuantity = NorthSlotQuantity;
		break;

	case EQuickSlotDirection::East:
		OutIcon = EastSlotIcon;
		OutQuantity = EastSlotQuantity;
		break;

	case EQuickSlotDirection::South:
		OutIcon = SouthSlotIcon;
		OutQuantity = SouthSlotQuantity;
		break;

	case EQuickSlotDirection::West:
		OutIcon = WestSlotIcon;
		OutQuantity = WestSlotQuantity;
		break;
	}
}

