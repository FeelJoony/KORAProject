// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KRSlotGridBase.h"
#include "UI/KRItemSlotBase.h"
//#include "UI/Data/KRItemUIData.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Groups/CommonButtonGroupBase.h"
#include "Blueprint/WidgetTree.h"

void UKRSlotGridBase::InitializeItemGrid(const TArray<FKRItemUIData>& InData)
{
	if (Slots.Num() == 0)
	{
		BuildGrid();
	}

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (UKRItemSlotBase* Cell = Slots[i])
		{
			if (InData.IsValidIndex(i))
			{
				Cell->SetIsEnabled(true);
				Cell->SetItemData(InData[i]);
			}
			else
			{
				Cell->SetItemData(FKRItemUIData{});
				Cell->SetIsEnabled(false);
			}
		}
	}
}

void UKRSlotGridBase::BuildGrid()
{
	if (!SlotGrid || !SlotClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("KRSlotGridBase::BuildGrid - SlotGrid or SlotClass is null."));
		return;
	}

	ClearGrid();

	const int32 SafeRows = FMath::Max(0, Rows);
	const int32 SafeCols = FMath::Max(0, Columns);
	const int32 Total = SafeRows * SafeCols;

	Slots.Reserve(Total);

	for (int32 i = 0; i < Total; ++i)
	{
		UKRItemSlotBase* Cell = CreateWidget<UKRItemSlotBase>(GetOwningPlayer(), SlotClass);
		if (!Cell) { continue; }

		const int32 Row = (SafeCols > 0) ? i / SafeCols : 0;
		const int32 Col = (SafeCols > 0) ? i % SafeCols : i;

		if (UUniformGridSlot* GridSlot = SlotGrid->AddChildToUniformGrid(Cell, Row, Col))
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
		BindSlot(Cell, i);
		Slots.Add(Cell);
	}
}

int32 UKRSlotGridBase::GetSelectedIndex() const
{
	if (!ButtonGroup) return INDEX_NONE;

	const int32 GroupIndex = ButtonGroup->GetSelectedButtonIndex();
	if (GroupIndex == INDEX_NONE) return INDEX_NONE;

	if (Slots.IsValidIndex(GroupIndex) && Slots[GroupIndex])
	{
		return GroupIndex;
	}
	return INDEX_NONE;
}

bool UKRSlotGridBase::SelectIndexSafe(int32 Index)
{
	if (!ButtonGroup) return false;
	if (!Slots.IsValidIndex(Index)) return false;

	ButtonGroup->SelectButtonAtIndex(Index);
	return true;
}

UWidget* UKRSlotGridBase::GetSelectedWidget() const
{
	return ButtonGroup ? ButtonGroup->GetSelectedButtonBase() : nullptr;
}

void UKRSlotGridBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ButtonGroup)
	{
		ButtonGroup = NewObject<UCommonButtonGroupBase>(this);
		ButtonGroup->SetSelectionRequired(true);
		ButtonGroup->OnHoveredButtonBaseChanged.AddDynamic(this, &ThisClass::HandleGroupHoveredChanged);
		ButtonGroup->OnSelectedButtonBaseChanged.AddDynamic(this, &ThisClass::HandleGroupSelectedChanged);
	}
}

void UKRSlotGridBase::NativeDestruct()
{
	if (ButtonGroup)
	{
		ButtonGroup->OnSelectedButtonBaseChanged.RemoveAll(this);
		ButtonGroup->OnHoveredButtonBaseChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UKRSlotGridBase::BindSlot(UKRItemSlotBase* CellWidget, int32 CellIndex)
{
	if (!CellWidget) return;

	if (ButtonGroup)
	{
		ButtonGroup->AddWidget(CellWidget);
	}
}

void UKRSlotGridBase::ClearGrid()
{
	if (SlotGrid)
	{
		SlotGrid->ClearChildren();
	}
	Slots.Reset();

	if (ButtonGroup)
	{
		ButtonGroup->DeselectAll();
	}
}

void UKRSlotGridBase::HandleGroupSelectedChanged(UCommonButtonBase* Selected, int32 SelectedIndex)
{
	UKRItemSlotBase* ItemSlot = Cast<UKRItemSlotBase>(Selected);
	OnSelectionChanged.Broadcast(SelectedIndex, ItemSlot);
}

void UKRSlotGridBase::HandleGroupHoveredChanged(UCommonButtonBase* Hovered, int32 HoveredIndex)
{
	UKRItemSlotBase* ItemSlot = Cast<UKRItemSlotBase>(Hovered);

	if (bSelectOnHover && ButtonGroup && HoveredIndex != INDEX_NONE)
	{
		ButtonGroup->SelectButtonAtIndex(HoveredIndex);
	}

	OnHoverChanged.Broadcast(HoveredIndex, ItemSlot);
}
