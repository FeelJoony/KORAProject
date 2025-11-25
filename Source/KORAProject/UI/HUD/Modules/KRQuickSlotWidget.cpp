// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/Modules/KRQuickSlotWidget.h"
#include "UI/HUD/Modules/KRQuickSlotButtonBase.h"

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

UKRQuickSlotButtonBase* UKRQuickSlotWidget::GetSlotButton(EQuickSlotDirection Direction) const
{
	switch (Direction)
	{
	case EQuickSlotDirection::North: return NorthQuickSlot;
	case EQuickSlotDirection::East:  return EastQuickSlot;
	case EQuickSlotDirection::South: return SouthQuickSlot;
	case EQuickSlotDirection::West:  return WestQuickSlot;
	default:                         return nullptr;
	}
}

void UKRQuickSlotWidget::UpdateSlot(EQuickSlotDirection Direction, int32 Quantity, const FSoftObjectPath& IconPath)
{
	if (UKRQuickSlotButtonBase* QuickSlot = GetSlotButton(Direction))
	{
		QuickSlot->BP_SetSlotData(Quantity, IconPath);
	}
}

void UKRQuickSlotWidget::ClearSlot(EQuickSlotDirection Direction)
{
	if (UKRQuickSlotButtonBase* QuickSlot = GetSlotButton(Direction))
	{
		QuickSlot->BP_ClearSlot();
	}
}

void UKRQuickSlotWidget::HighlightSlot(EQuickSlotDirection Direction)
{
	if (UKRQuickSlotButtonBase* N = GetSlotButton(EQuickSlotDirection::North)) N->BP_SetHighlight(false);
	if (UKRQuickSlotButtonBase* E = GetSlotButton(EQuickSlotDirection::East))  E->BP_SetHighlight(false);
	if (UKRQuickSlotButtonBase* S = GetSlotButton(EQuickSlotDirection::South)) S->BP_SetHighlight(false);
	if (UKRQuickSlotButtonBase* W = GetSlotButton(EQuickSlotDirection::West))  W->BP_SetHighlight(false);

	if (UKRQuickSlotButtonBase* Selected = GetSlotButton(Direction))
	{
		Selected->BP_SetHighlight(true);
	}
}

void UKRQuickSlotWidget::UpdateSlotQuantity(EQuickSlotDirection Direction, int32 NewQuantity)
{
	if (UKRQuickSlotButtonBase* QuickSlot = GetSlotButton(Direction))
	{
		QuickSlot->BP_UpdateQuantity(NewQuantity);
	}
}

