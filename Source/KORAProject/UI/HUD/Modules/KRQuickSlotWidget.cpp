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

	ClearSlot(FKRUIMessageTags::QuickSlot_North());
	ClearSlot(FKRUIMessageTags::QuickSlot_East());
	ClearSlot(FKRUIMessageTags::QuickSlot_South());
	ClearSlot(FKRUIMessageTags::QuickSlot_West());

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
	const FGameplayTag SlotDir = Message.CurrentlySelectedSlot;

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

UKRQuickSlotButtonBase* UKRQuickSlotWidget::GetSlotButton(FGameplayTag Direction) const
{
	if (Direction == FKRUIMessageTags::QuickSlot_North())
	{
		return NorthQuickSlot;
	}
	else if (Direction == FKRUIMessageTags::QuickSlot_East())
	{
		return EastQuickSlot;
	}
	else if (Direction == FKRUIMessageTags::QuickSlot_South())
	{
		return SouthQuickSlot;
	}
	else if (Direction == FKRUIMessageTags::QuickSlot_West())
	{
		return WestQuickSlot;
	}

	return nullptr;
}

void UKRQuickSlotWidget::UpdateSlot(FGameplayTag Direction, int32 Quantity, const TSoftObjectPtr<UTexture2D>& Icon)
{
	if (UKRQuickSlotButtonBase* QuickSlot = GetSlotButton(Direction))
	{
		QuickSlot->BP_SetSlotData(Quantity, Icon);
	}
}

void UKRQuickSlotWidget::ClearSlot(FGameplayTag Direction)
{
	if (UKRQuickSlotButtonBase* QuickSlot = GetSlotButton(Direction))
	{
		QuickSlot->BP_ClearSlot();
	}
}

void UKRQuickSlotWidget::HighlightSlot(FGameplayTag Direction)
{
	if (UKRQuickSlotButtonBase* N = GetSlotButton(FKRUIMessageTags::QuickSlot_North())) N->BP_SetHighlight(false);
	if (UKRQuickSlotButtonBase* E = GetSlotButton(FKRUIMessageTags::QuickSlot_East()))  E->BP_SetHighlight(false);
	if (UKRQuickSlotButtonBase* S = GetSlotButton(FKRUIMessageTags::QuickSlot_South())) S->BP_SetHighlight(false);
	if (UKRQuickSlotButtonBase* W = GetSlotButton(FKRUIMessageTags::QuickSlot_West()))  W->BP_SetHighlight(false);

	if (UKRQuickSlotButtonBase* Selected = GetSlotButton(Direction))
	{
		Selected->BP_SetHighlight(true);
	}
}

void UKRQuickSlotWidget::UpdateSlotQuantity(FGameplayTag Direction, int32 NewQuantity)
{
	if (UKRQuickSlotButtonBase* QuickSlot = GetSlotButton(Direction))
	{
		QuickSlot->BP_UpdateQuantity(NewQuantity);
	}
}

