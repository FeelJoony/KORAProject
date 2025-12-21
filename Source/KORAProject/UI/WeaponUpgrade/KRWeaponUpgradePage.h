// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "UI/Data/KRItemUIData.h"
#include "KRWeaponUpgradePage.generated.h"

class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UKRItemSlotBase;

UCLASS()
class KORAPROJECT_API UKRWeaponUpgradePage : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) UKRSlotGridBase* WeeaponSlot = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRItemDescriptionBase* WeaponDescriptionWidget = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRItemDescriptionBase* WeaponInfoSlot = nullptr;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY() TArray<FGameplayTag> WeaponSlotOrder;
	UPROPERTY() int32 SelectedWeaponIndex = 0;

	TArray<FKRItemUIData> CachedUIData;

	void InitializeWeaponSlotOrder();
	void RefreshWeaponSlots();

	//UFUNCTION() void OnWeaponSlotSelected(int32 CellIndex, UKRItemSlotBase* Slot);
	//UFUNCTION() void OnWeaponSlotHovered(int32 CellIndex, UKRItemSlotBase* Slot);

	void UpdateDescriptionUI(int32 CellIndex);
	void UpdateWeaponInfoUI(int32 CellIndex);

	void HandleSelect();
	void HandleNext();
	void HandlePrev();
};
