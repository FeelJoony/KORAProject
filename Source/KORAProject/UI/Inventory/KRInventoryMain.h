// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "UI/Data/KRItemUIData.h"
#include "KRInventoryMain.generated.h"

class UUniformGridPanel;
class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UCommonButtonBase;

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

	void BeginQuickSlotAssign(FGameplayTag SlotDir);

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	TArray<FKRItemUIData> CachedUIData;
	bool bQuickSlotAssignMode = false;
	FGameplayTag PendingQuickSlotDir;

	UFUNCTION() void OnClickConsumables();
	UFUNCTION() void OnClickMaterial();
	UFUNCTION() void OnClickQuest();

	UFUNCTION() void OnGridSlotSelected(int32 CellIndex);

	void FilterAndCacheItems(const FGameplayTag& FilterTag);
	void RebuildInventoryUI(const FGameplayTag& FilterTag);
	void BindInventoryEvents();
	void UpdateDescriptionUI(int32 CellIndex);

	void HandleMoveLeft();
	void HandleMoveRight();
	void HandleMoveUp();
	void HandleMoveDown();
	void HandleMoveInternal(uint8 DirIdx); // 0:L,1:R,2:U,3:D

	void HandleSelect();

	int32 StepGrid(int32 Cur, uint8 DirIdx, int32 Cols, int32 Num) const;
};
