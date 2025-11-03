// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/KRInventoryMain.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Data/KRItemUIData.h"

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

void UKRInventoryMain::SetupInputBindings()
{
	UnbindAll();
    BindBackDefault(Row_Back);
    //BindRow(Row_Prev, FSimpleDelegate::CreateUObject(this, &ThisClass::OnPrevItem));
    //BindRow(Row_Next, FSimpleDelegate::CreateUObject(this, &ThisClass::OnNextItem));
    //BindRow(Row_Select, FSimpleDelegate::CreateUObject(this, &ThisClass::OnSelectItem));
}

void UKRInventoryMain::OnClickConsumables()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagConsume = TM.RequestGameplayTag(TEXT("ItemType.Consume"));
	const FGameplayTag TagTool = TM.RequestGameplayTag(TEXT("ItemType.Tool"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagConsume, TagTool });
}

void UKRInventoryMain::OnClickMaterial()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagMaterial = TM.RequestGameplayTag(TEXT("ItemType.Material"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagMaterial });
}

void UKRInventoryMain::OnClickQuest()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagQuest = TM.RequestGameplayTag(TEXT("ItemType.Quest"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagQuest });
}

void UKRInventoryMain::OnGridSlotSelected(int32 CellIndex)
{
	UpdateDescriptionUI(CellIndex);
}

void UKRInventoryMain::FilterAndCacheItems(const FGameplayTag& FilterTag)
{
	/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/
}

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