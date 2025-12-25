#include "UI/KRSlotGridBase.h"
#include "UI/KRItemSlotBase.h"
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

	if (ButtonGroup && Slots.Num() > 0)
	{
		ButtonGroup->SelectButtonAtIndex(0);
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

bool UKRSlotGridBase::HoverIndexSafe(int32 Index)
{
	if (!Slots.IsValidIndex(Index)) return false;

	HoveredIndex = Index;

	OnHoverChanged.Broadcast(Index, Slots[Index]);
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

void UKRSlotGridBase::RebindButtonGroup()
{
    if (ButtonGroup)
    {
        ButtonGroup->OnSelectedButtonBaseChanged.RemoveAll(this);
        ButtonGroup->OnHoveredButtonBaseChanged.RemoveAll(this);
    }

    ButtonGroup = NewObject<UCommonButtonGroupBase>(this);
    ButtonGroup->SetSelectionRequired(true);

    ButtonGroup->OnHoveredButtonBaseChanged.AddDynamic(this, &ThisClass::HandleGroupHoveredChanged);
    ButtonGroup->OnSelectedButtonBaseChanged.AddDynamic(this, &ThisClass::HandleGroupSelectedChanged);

    for (UKRItemSlotBase* ItemSlot : Slots)
    {
        if (ItemSlot)
        {
            ButtonGroup->AddWidget(ItemSlot);
        }
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

	TWeakObjectPtr<UKRItemSlotBase> WeakSlot = CellWidget;

    CellWidget->OnClicked().AddWeakLambda(this, [this, CellIndex]()
    {
        if (!bSelectOnHover && ButtonGroup)
        {
            ButtonGroup->SelectButtonAtIndex(CellIndex);
        }

        OnSlotClicked.Broadcast();
    });
}

void UKRSlotGridBase::ClearGrid()
{
	if (ButtonGroup)
	{
		for (UKRItemSlotBase* SlotBase : Slots)
		{
			if (SlotBase)
			{
				ButtonGroup->RemoveWidget(SlotBase);
			}
		}
	}

	if (SlotGrid)
	{
		SlotGrid->ClearChildren();
	}
	Slots.Reset();
	HoveredIndex = INDEX_NONE;
	LastHoveredIndex = INDEX_NONE;
}

void UKRSlotGridBase::HandleGroupSelectedChanged(UCommonButtonBase* Selected, int32 SelectedIndex)
{
	UKRItemSlotBase* ItemSlot = Cast<UKRItemSlotBase>(Selected);
	OnSelectionChanged.Broadcast(SelectedIndex, ItemSlot);
}

void UKRSlotGridBase::HandleGroupHoveredChanged(UCommonButtonBase* Hovered, int32 InHoveredIndex)
{
	if (Slots.IsValidIndex(LastHoveredIndex) && Slots[LastHoveredIndex])
	{
		Slots[LastHoveredIndex]->SetHovered(false);
	}

	if (Slots.IsValidIndex(InHoveredIndex) && Slots[InHoveredIndex])
	{
		Slots[InHoveredIndex]->SetHovered(true);
	}

	LastHoveredIndex = InHoveredIndex;
	HoveredIndex = InHoveredIndex;

	if (bSelectOnHover && ButtonGroup && InHoveredIndex != INDEX_NONE)
	{
		ButtonGroup->SelectButtonAtIndex(InHoveredIndex);
	}

	OnHoverChanged.Broadcast(InHoveredIndex, Cast<UKRItemSlotBase>(Hovered));
}