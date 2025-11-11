// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponUpgrade/KRWeaponUpgradePage.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Data/KRItemUIData.h"
#include "SubSystem/KRUIInputSubsystem.h"

/*
#include "Inventory/KRInventoryComponent.h"   // 인벤토리 컴포넌트 받고 수정
#include "Item/KRBaseItem.h"                 // 아이템 받고 수정
*/

// 필요 함수 리스트 !! 
/* 현재 위젯 소유자 Pawn에서 인벤토리 컴포넌트 가져오는 함수 */
/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/

void UKRWeaponUpgradePage::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("Inventory"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandlePrev));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleNext));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleNext));
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandlePrev));
	}
}

void UKRWeaponUpgradePage::NativeOnDeactivated()
{
	//if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	//{
	//	InputSubsys->UnbindAll(this);
	//}
	Super::NativeOnDeactivated();
}

void UKRWeaponUpgradePage::OnGridSlotSelected(int32 CellIndex)
{
	UpdateDescriptionUI(CellIndex);
}

void UKRWeaponUpgradePage::FilterAndCacheWeapons(const FGameplayTag& FilterTag)
{
	/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/
}

void UKRWeaponUpgradePage::UpdateDescriptionUI(int32 CellIndex)
{
	if (!WeaponDescriptionWidget) return;
	if (!WeaponInfoSlot) return;

	if (CachedUIData.IsValidIndex(CellIndex))
	{
		WeaponDescriptionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedUIData[CellIndex];
		WeaponDescriptionWidget->UpdateItemInfo(D.ItemName, D.ItemDescription, D.ItemIcon);
		WeaponInfoSlot->UpdateItemInfo(D.ItemName, D.ItemDescription, D.ItemIcon, D.UpgradeLevel);
	}
	else
	{
		WeaponDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
		WeaponInfoSlot->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRWeaponUpgradePage::HandleSelect()
{
}

void UKRWeaponUpgradePage::HandleNext()
{
}

void UKRWeaponUpgradePage::HandlePrev()
{
}
