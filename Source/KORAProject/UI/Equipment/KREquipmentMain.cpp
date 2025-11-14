// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Equipment/KREquipmentMain.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"

#include "CommonButtonBase.h"
#include "GameplayTagsManager.h"
/*
#include "Inventory/KRInventoryComponent.h"   // �κ��丮 ������Ʈ �ް� ����
#include "Item/KRBaseItem.h"                 // ������ �ް� ����
*/

// �ʿ� �Լ� ����Ʈ !! 
/* ���� ���� ������ Pawn���� �κ��丮 ������Ʈ �������� �Լ� */
/* �ױ� ��Ī���� ������ �����ͼ� UIData�� ��ȯ�ϴ� �Լ�*/

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

void UKREquipmentMain::SetupInputBindings()
{
	UnbindAll();
	BindBackDefault(Row_Back);
	//BindRow(Row_Prev, FSimpleDelegate::CreateUObject(this, &ThisClass::OnPrevItem));
	//BindRow(Row_Next, FSimpleDelegate::CreateUObject(this, &ThisClass::OnNextItem));
	//BindRow(Row_Select, FSimpleDelegate::CreateUObject(this, &ThisClass::OnSelectItem));
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
	/* �ױ� ��Ī���� ������ �����ͼ� UIData�� ��ȯ�ϴ� �Լ�*/
}

void UKREquipmentMain::RebuildInventoryUI(const TArray<FGameplayTag>& TagsAny)
{
	/* �ױ� ��Ī���� ������ �����ͼ� UIData�� ��ȯ�ϴ� �Լ�*/
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
