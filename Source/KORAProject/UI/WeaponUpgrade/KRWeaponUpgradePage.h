// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "KRWeaponUpgradePage.generated.h"

class UKRSlotGridBase;
class UKRItemDescriptionBase;
struct FKRItemUIData;

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

private:
	//TWeakObjectPtr<UKRInventoryComponent> InventoryComp;
	TArray<FKRItemUIData> CachedUIData;

	UFUNCTION() void OnGridSlotSelected(int32 CellIndex);

	void FilterAndCacheWeapons(const FGameplayTag& FilterTag);

	void UpdateDescriptionUI(int32 CellIndex);

	void HandleSelect();
	void HandleNext();
	void HandlePrev();
};
