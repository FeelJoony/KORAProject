// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/KRInventoryMain.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Data/KRItemUIData.h"
#include "SubSystem/KRUIInputSubsystem.h"

#include "CommonButtonBase.h"
#include "GameplayTagsManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
/*
#include "Inventory/KRInventoryComponent.h"   // 인벤토리 컴포넌트 받고 수정
#include "Item/KRBaseItem.h"                 // 아이템 받고 수정
*/

// 필요 함수 리스트 !! 
/* 현재 위젯 소유자 Pawn에서 인벤토리 컴포넌트 가져오는 함수 */
/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/

void UKRInventoryMain::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->EnterUIMode();
		InputSubsys->BindBackDefault(this, TEXT("Inventory"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDown));
	}
}

void UKRInventoryMain::NativeOnDeactivated()
{
	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->ReleaseUIMode();
	}
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
		//InventorySlot->OnSlotSelected.AddDynamic(this, &UKRInventoryMain::OnGridSlotSelected);
	}

	//BindInventoryEvents();
	OnClickConsumables();
}

void UKRInventoryMain::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKRInventoryMain::OnClickConsumables() { RebuildByTag(TEXT("ItemType.Consume")); }
void UKRInventoryMain::OnClickMaterial() { RebuildByTag(TEXT("ItemType.Material")); }
void UKRInventoryMain::OnClickQuest() { RebuildByTag(TEXT("ItemType.Quest")); }

void UKRInventoryMain::RebuildByTag(const FName& TagName)
{
	const FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TagName);
	RebuildInventoryUI({ Tag });
}

void UKRInventoryMain::OnGridSlotSelected(int32 CellIndex)
{
	UpdateDescriptionUI(CellIndex);
}

//void UKRInventoryMain::FilterAndCacheItems(const FGameplayTag& FilterTag)
//{
//	/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/
//}

void UKRInventoryMain::RebuildInventoryUI(const TArray<FGameplayTag>& TagsAny)
{
	/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/

	if (InventorySlot)
	{
		InventorySlot->InitializeItemGrid(CachedUIData);

		if (CachedUIData.Num() > 0)
		{
			UpdateDescriptionUI(0);
		}
		else
		{
			UpdateDescriptionUI(INDEX_NONE);
		}
	}
}

//void UKRInventoryMain::BindInventoryEvents()
//{
//	// 인벤토리 변경되면 UI에 반영 할 때 -> 필요 시에 구현 
//}

void UKRInventoryMain::UpdateDescriptionUI(int32 CellIndex)
{
	if (!ItemDescriptionWidget) return;

	if (CachedUIData.IsValidIndex(CellIndex))
	{
		ItemDescriptionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedUIData[CellIndex];
		ItemDescriptionWidget->UpdateItemInfo(D.ItemName, D.ItemDescription, D.ItemIcon);
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
		UpdateDescriptionUI(Next);
	}
}

void UKRInventoryMain::HandleSelect()
{
	// For QuickSlot
}

int32 UKRInventoryMain::StepGrid(int32 Cur, uint8 DirIdx, int32 Cols, int32 Num) const
{
	if (Cols <= 0 || Num <= 0) return 0;
	if (Cur < 0) Cur = 0;

	switch (DirIdx)
	{
	case 0: // Left
	{
		const int32 Col = Cur % Cols;
		return (Col > 0) ? (Cur - 1) : Cur;
	}
	case 1: // Right
	{
		const int32 Col = Cur % Cols;
		const bool bAtRight = (Col == Cols - 1) || (Cur + 1 >= Num);
		return bAtRight ? Cur : (Cur + 1);
	}
	case 2: // Up
		return (Cur - Cols >= 0) ? (Cur - Cols) : Cur;

	case 3: // Down
		return (Cur + Cols < Num) ? (Cur + Cols) : Cur;

	default:
		return Cur;
	}
}