// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "KRInventoryMain.generated.h"

class UUniformGridPanel;
class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UCommonButtonBase;
struct FKRItemUIData;
// 인벤토리 컴포넌트

UCLASS()
class KORAPROJECT_API UKRInventoryMain : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* ConsumablesButton;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* MaterialButton;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* QuestButton;

	UPROPERTY(meta = (BindWidget)) UKRSlotGridBase* InventorySlot = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRItemDescriptionBase* ItemDescriptionWidget = nullptr;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	//TWeakObjectPtr<UKRInventoryComponent> InventoryComp;
	TArray<FKRItemUIData> CachedUIData;

	UFUNCTION() void OnClickConsumables();
	UFUNCTION() void OnClickMaterial();
	UFUNCTION() void OnClickQuest();

	UFUNCTION() void OnGridSlotSelected(int32 CellIndex);

	void FilterAndCacheItems(const FGameplayTag& FilterTag);
	void RebuildInventoryUI(const TArray<FGameplayTag>& TagsAny);

	//void BindInventoryEvents();
	void UpdateDescriptionUI(int32 CellIndex);


	void HandleSelect();
	void HandleNext();
	void HandlePrev();
};
