// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/KRInventoryMain.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "SubSystem/KRUIInputSubsystem.h"

#include "CommonButtonBase.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UKRInventoryMain::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("Inventory"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDown));
	}
	RebuildInventoryUI(KRTAG_ITEMTYPE_CONSUME);
}

void UKRInventoryMain::NativeOnDeactivated()
{
	if (ConsumablesButton) ConsumablesButton->OnClicked().RemoveAll(this);
	if (MaterialButton)    MaterialButton->OnClicked().RemoveAll(this);
	if (QuestButton)       QuestButton->OnClicked().RemoveAll(this);

	Super::NativeOnDeactivated();
}

void UKRInventoryMain::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConsumablesButton) ConsumablesButton->OnClicked().AddUObject(this, &UKRInventoryMain::OnClickConsumables);
	if (MaterialButton)    MaterialButton->OnClicked().AddUObject(this, &UKRInventoryMain::OnClickMaterial);
	if (QuestButton)       QuestButton->OnClicked().AddUObject(this, &UKRInventoryMain::OnClickQuest);

	if (InventorySlot)
	{
		InventorySlot->bSelectOnHover = true;
		InventorySlot->OnSelectionChanged.AddDynamic(this, &ThisClass::OnGridSlotSelected);
		InventorySlot->OnHoverChanged.AddDynamic(this, &ThisClass::OnGridSlotHovered);
		InventorySlot->OnSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
	}

	OnClickConsumables();
}

void UKRInventoryMain::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DescriptionUpdateTimerHandle);
	}

	if (ConsumablesButton) ConsumablesButton->OnClicked().RemoveAll(this);
	if (MaterialButton)    MaterialButton->OnClicked().RemoveAll(this);
	if (QuestButton)       QuestButton->OnClicked().RemoveAll(this);

	if (InventorySlot)
	{
		InventorySlot->OnSelectionChanged.RemoveAll(this);
		InventorySlot->OnHoverChanged.RemoveAll(this);
		InventorySlot->OnSlotClicked.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UKRInventoryMain::OnClickConsumables() { RebuildInventoryUI(KRTAG_ITEMTYPE_CONSUME); }
void UKRInventoryMain::OnClickMaterial() { RebuildInventoryUI(KRTAG_ITEMTYPE_MATERIAL); }
void UKRInventoryMain::OnClickQuest() { RebuildInventoryUI(KRTAG_ITEMTYPE_QUEST); }

void UKRInventoryMain::OnGridSlotSelected(int32 CellIndex, UKRItemSlotBase* SlotBase)
{
	UpdateDescriptionUI(CellIndex);
}

void UKRInventoryMain::OnGridSlotHovered(int32 CellIndex, UKRItemSlotBase* SlotBase)
{
	UpdateDescriptionUI(CellIndex);
}

void UKRInventoryMain::OnGridSlotClicked()
{
	HandleSelect();
}

void UKRInventoryMain::FilterAndCacheItems(const FGameplayTag& FilterTag)
{
	UKRUIAdapterLibrary::GetInventoryUIDataFiltered(this, FilterTag, CachedUIData);
}

void UKRInventoryMain::RebuildInventoryUI(const FGameplayTag& FilterTag)
{
	CurrentFilterTag = FilterTag;
	FilterAndCacheItems(FilterTag);

	if (InventorySlot)
	{
		InventorySlot->InitializeItemGrid(CachedUIData);

		if (CachedUIData.Num() > 0)
		{
			InventorySlot->RebindButtonGroup();
			InventorySlot->SelectIndexSafe(0);
		}
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(DescriptionUpdateTimerHandle);
			World->GetTimerManager().SetTimer(
				DescriptionUpdateTimerHandle,
				this,
				&UKRInventoryMain::DelayedInitialDescriptionUpdate,
				0.01f,
				false
			);
		}
	}
}

void UKRInventoryMain::DelayedInitialDescriptionUpdate()
{
	if (CachedUIData.Num() > 0)
	{
		UpdateDescriptionUI(0);
	}
	else
	{
		UpdateDescriptionUI(INDEX_NONE);
	}
}

void UKRInventoryMain::BindInventoryEvents()
{
	// When Inventory's updated...
}

void UKRInventoryMain::UpdateDescriptionUI(int32 CellIndex)
{
	if (!ItemDescriptionWidget) return;

	if (CachedUIData.IsValidIndex(CellIndex))
	{
		ItemDescriptionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedUIData[CellIndex];
		ItemDescriptionWidget->UpdateItemInfo(D.ItemNameKey, D.ItemDescriptionKey, D.ItemIcon);
	}
	else
	{
		ItemDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRInventoryMain::HandleMoveLeft() { HandleMoveInternal(0); }
void UKRInventoryMain::HandleMoveRight() { HandleMoveInternal(1); }
void UKRInventoryMain::HandleMoveUp() { HandleMoveInternal(2); }
void UKRInventoryMain::HandleMoveDown() { HandleMoveInternal(3); }

void UKRInventoryMain::HandleMoveInternal(uint8 DirIdx)
{
	if (!InventorySlot) return;

	const int32 Cols = InventorySlot->GetColumnCount();
	const int32 Num = InventorySlot->GetNumCells();
	if (Cols <= 0 || Num <= 0) return;

	const int32 Cur = InventorySlot->GetSelectedIndex();
	const int32 Next = StepGrid(Cur, DirIdx, Cols, Num);

	if (Next != Cur)
	{
		InventorySlot->SelectIndexSafe(Next);
		if (UWidget* W = InventorySlot->GetSelectedWidget()) W->SetFocus();
	}
}

void UKRInventoryMain::HandleSelect()
{

}

int32 UKRInventoryMain::StepGrid(int32 Cur, uint8 DirIdx, int32 Cols, int32 Num) const
{
	if (Num <= 0 || Cols <= 0)
	{
		return Cur;
	}

	int32 Row = Cur / Cols;
	int32 Col = Cur % Cols;

	switch (DirIdx)
	{
	case 0:  // Left
		Col = (Col > 0) ? (Col - 1) : Col;
		break;
	case 1:  // Right
		Col = (Col < Cols - 1) ? (Col + 1) : Col;
		break;
	case 2:  // Up
		Row = (Row > 0) ? (Row - 1) : Row;
		break;
	case 3:  // Down
	{
		int32 MaxRow = (Num - 1) / Cols;
		Row = (Row < MaxRow) ? (Row + 1) : Row;
		break;
	}
	default:
		break;
	}

	int32 Next = Row * Cols + Col;
	if (Next >= Num) Next = Num - 1;

	return FMath::Clamp(Next, 0, Num - 1);
}