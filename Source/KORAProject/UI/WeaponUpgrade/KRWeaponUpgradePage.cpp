// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponUpgrade/KRWeaponUpgradePage.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "GameplayTag/KRItemTypeTag.h"

void UKRWeaponUpgradePage::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeWeaponSlotOrder();

	//if (WeeaponSlot)
	//{
	//	WeeaponSlot->OnSelectionChanged.AddDynamic(this, &ThisClass::OnWeaponSlotSelected);
	//	WeeaponSlot->OnHoverChanged.AddDynamic(this, &ThisClass::OnWeaponSlotHovered);
	//}

	RefreshWeaponSlots();

	if (WeeaponSlot && CachedUIData.Num() > 0)
	{
		WeeaponSlot->SelectIndexSafe(0);
	}
}

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

	RefreshWeaponSlots();

	if (WeeaponSlot)
	{
		const int32 Safe = FMath::Clamp(SelectedWeaponIndex, 0, CachedUIData.Num() - 1);
		WeeaponSlot->SelectIndexSafe(Safe);
	}
}

void UKRWeaponUpgradePage::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void UKRWeaponUpgradePage::InitializeWeaponSlotOrder()
{
	WeaponSlotOrder = {
		KRTAG_ITEMTYPE_EQUIP_GUN,
		KRTAG_ITEMTYPE_EQUIP_SWORD
	};
}

void UKRWeaponUpgradePage::RefreshWeaponSlots()
{
	CachedUIData.Empty();
	
	for (const FGameplayTag& SlotTag : WeaponSlotOrder)
	{
		FKRItemUIData UIData;
		if (UKRUIAdapterLibrary::GetEquippedSlotUIData(this, SlotTag, UIData))
		{
			CachedUIData.Add(UIData);
		}
		else
		{
			CachedUIData.Add(FKRItemUIData());
		}
	}

	if (WeeaponSlot)
	{
		WeeaponSlot->InitializeItemGrid(CachedUIData);
	}
	
	if (CachedUIData.Num() > 0)
	{
		UpdateDescriptionUI(0);
		UpdateWeaponInfoUI(0);
	}
}

//void UKRWeaponUpgradePage::OnWeaponSlotSelected(int32 CellIndex, UKRItemSlotBase* Slot)
//{
//	SelectedWeaponIndex = CellIndex;
//	UpdateDescriptionUI(CellIndex);
//	UpdateWeaponInfoUI(CellIndex);
//}
//
//void UKRWeaponUpgradePage::OnWeaponSlotHovered(int32 CellIndex, UKRItemSlotBase* Slot)
//{
//	UpdateDescriptionUI(CellIndex);
//	UpdateWeaponInfoUI(CellIndex);
//}

void UKRWeaponUpgradePage::UpdateDescriptionUI(int32 CellIndex)
{
	if (!WeaponDescriptionWidget) return;

	if (CachedUIData.IsValidIndex(CellIndex) && CachedUIData[CellIndex].ItemTag.IsValid())
	{
		WeaponDescriptionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedUIData[CellIndex];
		WeaponDescriptionWidget->UpdateItemInfo(D.ItemNameKey, D.ItemDescriptionKey, D.ItemIcon);
	}
	else
	{
		WeaponDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRWeaponUpgradePage::UpdateWeaponInfoUI(int32 CellIndex)
{
	if (!WeaponInfoSlot) return;

	if (CachedUIData.IsValidIndex(CellIndex) && CachedUIData[CellIndex].ItemTag.IsValid())
	{
		WeaponInfoSlot->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedUIData[CellIndex];
		
		const int32 DisplayLevel = D.UpgradeLevel > 0 ? D.UpgradeLevel : 1;
		WeaponInfoSlot->UpdateItemInfo(D.ItemNameKey, D.ItemDescriptionKey, D.ItemIcon, DisplayLevel);
	}
	else
	{
		WeaponInfoSlot->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRWeaponUpgradePage::HandleSelect()
{
	// Weapon Upgrade Logic Here
}

void UKRWeaponUpgradePage::HandleNext()
{
	if (!WeeaponSlot) return;

	const int32 Num = WeeaponSlot->GetNumCells();
	if (Num <= 0) return;

	const int32 Next = (SelectedWeaponIndex + 1) % Num;
	WeeaponSlot->SelectIndexSafe(Next);
}

void UKRWeaponUpgradePage::HandlePrev()
{
	if (!WeeaponSlot) return;

	const int32 Num = WeeaponSlot->GetNumCells();
	if (Num <= 0) return;

	const int32 Prev = (SelectedWeaponIndex - 1 + Num) % Num;
	WeeaponSlot->SelectIndexSafe(Prev);
}
