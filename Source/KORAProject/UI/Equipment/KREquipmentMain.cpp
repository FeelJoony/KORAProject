// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Equipment/KREquipmentMain.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Data/KRItemUIData.h"
#include "SubSystem/KRUIInputSubsystem.h"

#include "CommonButtonBase.h"
#include "GameplayTagsManager.h"
/*
#include "Inventory/KRInventoryComponent.h"   // 인벤토리 컴포넌트 받고 수정
#include "Item/KRBaseItem.h"                 // 아이템 받고 수정
*/

// 필요 함수 리스트 !! 
/* 현재 위젯 소유자 Pawn에서 인벤토리 컴포넌트 가져오는 함수 */
/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/

void UKREquipmentMain::NativeOnActivated()
{
	Super::NativeOnActivated();

	UE_LOG(LogTemp, Log, TEXT("NativeOnActivated... Equipment"));
	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("Equipment"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleNext));
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandlePrev));
	}
}

void UKREquipmentMain::NativeOnDeactivated()
{
	UE_LOG(LogTemp, Log, TEXT("NativeOnDeactivated... Equipment"));
	//if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	//{
	//	InputSubsys->UnbindAll(this);
	//}
	Super::NativeOnDeactivated();
}

void UKREquipmentMain::NativeConstruct()
{
	if (SwordModuleSelect)	SwordModuleSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickSwordModule);
	if (SwordSkinSelect)		SwordSkinSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickSwordSkin);
	if (GunModuleSelect)		GunModuleSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickGunModule);
	if (GunSkinSelect)		GunSkinSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickGunSkin);
	if (HeadColorSelect)		HeadColorSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickHeadColor);
	if (DressColorSelect)	DressColorSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickDressColor);
	if (InventorySlot)
	{
		//InventorySlot->OnSlotSelected.AddDynamic(this, &UKREquipmentMain::OnGridSlotSelected);
	}

	//BindInventoryEvents();
	OnClickSwordModule();
}

void UKREquipmentMain::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKREquipmentMain::OnClickSwordModule()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagSwordModule = TM.RequestGameplayTag(TEXT("ItemType.Equip.SwordModule"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagSwordModule });
}

void UKREquipmentMain::OnClickSwordSkin()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagSwordSkin = TM.RequestGameplayTag(TEXT("ItemType.Equip.Sword"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagSwordSkin });
}

void UKREquipmentMain::OnClickGunModule()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagGunModule = TM.RequestGameplayTag(TEXT("ItemType.Equip.GunModule"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagGunModule });
}

void UKREquipmentMain::OnClickGunSkin()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagGunSkin = TM.RequestGameplayTag(TEXT("ItemType.Equip.Gun"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagGunSkin });
}

void UKREquipmentMain::OnClickHeadColor()
{
	auto& TM = UGameplayTagsManager::Get();	
	const FGameplayTag TagHeadColor = TM.RequestGameplayTag(TEXT("ItemType.Equip.Head"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagHeadColor });
}

void UKREquipmentMain::OnClickDressColor()
{
	auto& TM = UGameplayTagsManager::Get();
	const FGameplayTag TagDressColor = TM.RequestGameplayTag(TEXT("ItemType.Equip.Costume"));
	RebuildInventoryUI(TArray<FGameplayTag>{ TagDressColor });
}

void UKREquipmentMain::OnGridSlotSelected(int32 CellIndex)
{
	UpdateDescriptionUI(CellIndex);
}

void UKREquipmentMain::FilterAndCacheItems(const FGameplayTag& FilterTag)
{
	/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/
}

void UKREquipmentMain::RebuildInventoryUI(const TArray<FGameplayTag>& TagsAny)
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

void UKREquipmentMain::UpdateDescriptionUI(int32 CellIndex)
{
	if (!ModuleDescription) return;

	if (CachedUIData.IsValidIndex(CellIndex))
	{
		ModuleDescription->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedUIData[CellIndex];
		ModuleDescription->UpdateItemInfo(D.ItemName, D.ItemDescription, D.ItemIcon);
	}
	else
	{
		ModuleDescription->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKREquipmentMain::HandleSelect()
{
}

void UKREquipmentMain::HandleNext()
{
}

void UKREquipmentMain::HandlePrev()
{
}
