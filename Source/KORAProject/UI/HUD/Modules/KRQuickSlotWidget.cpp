#include "UI/HUD/Modules/KRQuickSlotWidget.h"
#include "UI/HUD/Modules/KRQuickSlotButtonBase.h"
#include "UI/Data/KRItemUIData.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "Engine/World.h"
#include "Groups/CommonButtonGroupBase.h"

void UKRQuickSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize QuickSlot ButtonGroup
	QuickSlotButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	QuickSlotButtonGroup->SetSelectionRequired(false);

	// Add all slot buttons to the ButtonGroup
	if (NorthQuickSlot) QuickSlotButtonGroup->AddWidget(NorthQuickSlot);
	if (EastQuickSlot) QuickSlotButtonGroup->AddWidget(EastQuickSlot);
	if (SouthQuickSlot) QuickSlotButtonGroup->AddWidget(SouthQuickSlot);
	if (WestQuickSlot) QuickSlotButtonGroup->AddWidget(WestQuickSlot);

	if (bListenGameplayMessages)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

			QuickSlotListener = Subsys.RegisterListener(
				FKRUIMessageTags::QuickSlot(),
				this,
				&ThisClass::OnQuickSlotMessageReceived
			);
		}
	}

	ClearSlot(FKRUIMessageTags::QuickSlot_North());
	ClearSlot(FKRUIMessageTags::QuickSlot_East());
	ClearSlot(FKRUIMessageTags::QuickSlot_South());
	ClearSlot(FKRUIMessageTags::QuickSlot_West());

	RefreshFromInventory();
	HighlightSlot(FKRUIMessageTags::QuickSlot_North());
}

void UKRQuickSlotWidget::NativeDestruct()
{
	if (bListenGameplayMessages)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
			Subsys.UnregisterListener(QuickSlotListener);
		}
	}

	Super::NativeDestruct();
}

void UKRQuickSlotWidget::RefreshFromInventory()
{
	TArray<FGameplayTag> Dirs = {
		FKRUIMessageTags::QuickSlot_North(),
		FKRUIMessageTags::QuickSlot_East(),
		FKRUIMessageTags::QuickSlot_South(),
		FKRUIMessageTags::QuickSlot_West()
	};

	for (const auto& Dir : Dirs)
	{
		FKRItemUIData Data;
		if (UKRUIAdapterLibrary::GetQuickSlotUIData(this, Dir, Data))
		{
			UpdateSlot(Dir, Data.Quantity, Data.ItemIcon);
		}
		else
		{
			ClearSlot(Dir);
		}
	}
}

void UKRQuickSlotWidget::NotifySlotHovered(FGameplayTag SlotDir)
{
	OnSlotHovered.Broadcast(SlotDir);
}

void UKRQuickSlotWidget::NotifySlotClicked()
{
	OnSlotClicked.Broadcast();
}

void UKRQuickSlotWidget::OnQuickSlotMessageReceived(FGameplayTag Channel, const FKRUIMessage_QuickSlot& Message)
{
	const FGameplayTag SlotDir = Message.CurrentlySelectedSlot;

	switch (Message.ActionType)
	{
	case EQuickSlotAction::ItemRegistered:
	{
		UpdateSlot(SlotDir, Message.ItemQuantity, Message.ItemIcon);
		BP_OnSlotRegistered(SlotDir);
		break;
	}

	case EQuickSlotAction::ItemUnregistered:
	{
		ClearSlot(SlotDir);
		BP_OnSlotUnregistered(SlotDir);
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
		BP_ResetAllSlotFrames();
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
	if (UKRQuickSlotButtonBase* N = GetSlotButton(FKRUIMessageTags::QuickSlot_North())) N->BP_SetHighlight(UnHightlightedSlotOpacity);
	if (UKRQuickSlotButtonBase* E = GetSlotButton(FKRUIMessageTags::QuickSlot_East()))  E->BP_SetHighlight(UnHightlightedSlotOpacity);
	if (UKRQuickSlotButtonBase* S = GetSlotButton(FKRUIMessageTags::QuickSlot_South())) S->BP_SetHighlight(UnHightlightedSlotOpacity);
	if (UKRQuickSlotButtonBase* W = GetSlotButton(FKRUIMessageTags::QuickSlot_West()))  W->BP_SetHighlight(UnHightlightedSlotOpacity);

	if (UKRQuickSlotButtonBase* Selected = GetSlotButton(Direction))
	{
		Selected->BP_SetHighlight(HighlightedSlotOpacity);
	}
}

void UKRQuickSlotWidget::UpdateSlotQuantity(FGameplayTag Direction, int32 NewQuantity)
{
	if (UKRQuickSlotButtonBase* QuickSlot = GetSlotButton(Direction))
	{
		QuickSlot->BP_UpdateQuantity(NewQuantity);
	}

	if (NewQuantity <= 0)
	{
		if (UKRQuickSlotButtonBase* Selected = GetSlotButton(Direction))
		{
			Selected->BP_SetHighlight(QuantityZeroSlotOpacity);
		}
	}
}

FGameplayTag UKRQuickSlotWidget::GetHoveredSlot() const
{
	TArray<TPair<FGameplayTag, UKRQuickSlotButtonBase*>> Slots = {
		{ FKRUIMessageTags::QuickSlot_North(), NorthQuickSlot },
		{ FKRUIMessageTags::QuickSlot_East(), EastQuickSlot },
		{ FKRUIMessageTags::QuickSlot_South(), SouthQuickSlot },
		{ FKRUIMessageTags::QuickSlot_West(), WestQuickSlot }
	};

	for (const auto& Pair : Slots)
	{
		if (Pair.Value && Pair.Value->IsHovered())
		{
			return Pair.Key;
		}
	}

	return FGameplayTag();
}

void UKRQuickSlotWidget::SetSelectedSlot(FGameplayTag SlotDir, bool bUpdateVisuals)
{
	CurrentSelectedSlot = SlotDir;

	if (bUpdateVisuals)
	{
		// Use ButtonGroup to manage selection state
		if (QuickSlotButtonGroup)
		{
			int32 SlotIndex = GetSlotIndex(SlotDir);
			if (SlotIndex != INDEX_NONE)
			{
				QuickSlotButtonGroup->SelectButtonAtIndex(SlotIndex);
			}
		}

		HighlightSlot(SlotDir);
	}
}

void UKRQuickSlotWidget::FocusSlot(FGameplayTag SlotDir)
{
	SetSelectedSlot(SlotDir, true);
	
	if (UKRQuickSlotButtonBase* ButtonToFocus = GetSlotButton(SlotDir))
	{
		ButtonToFocus->SetFocus();
	}
	
	NotifySlotHovered(SlotDir);
}

void UKRQuickSlotWidget::ClearAllSelections()
{
	if (QuickSlotButtonGroup)
	{
		QuickSlotButtonGroup->DeselectAll();
	}
}

int32 UKRQuickSlotWidget::GetSlotIndex(FGameplayTag SlotDir) const
{
	if (SlotDir == FKRUIMessageTags::QuickSlot_North())
	{
		return 0;
	}
	else if (SlotDir == FKRUIMessageTags::QuickSlot_East())
	{
		return 1;
	}
	else if (SlotDir == FKRUIMessageTags::QuickSlot_South())
	{
		return 2;
	}
	else if (SlotDir == FKRUIMessageTags::QuickSlot_West())
	{
		return 3;
	}

	return INDEX_NONE;
}
