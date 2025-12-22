// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "UI/Data/KRItemUIData.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KREquipmentMain.generated.h"

class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UKRItemSlotBase;

UENUM(BlueprintType)
enum class EFocusRegion : uint8
{
	Category,
	Grid
};

UENUM()
enum class ENavDir : uint8 { L, R, U, D };

UCLASS()
class KORAPROJECT_API UKREquipmentMain : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) UKRSlotGridBase* EquipCategorySlot = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRSlotGridBase* EquipInventorySlot = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRItemDescriptionBase* ModuleDescription = nullptr;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY() TArray<FGameplayTag> CategorySlotOrder;
	UPROPERTY() TArray<int32> LastInventoryIndexPerCategory;
	UPROPERTY() int32 ActiveCategoryIndex = 0;
	EFocusRegion FocusRegion = EFocusRegion::Category;

	TArray<FKRItemUIData> InventorySlotUIData;
	FGameplayMessageListenerHandle EquipMessageHandle;
	FGameplayMessageListenerHandle ConfirmMessageHandle;

	void InitializeCategoryOrder();
	void RefreshEquippedCategoryIcons();
	void FilterAndCacheItems(const FGameplayTag& FilterTag);
	void RebuildInventoryUI(const FGameplayTag& FilterTag);
	void UpdateDescriptionUI(int32 CellIndex);
	void UpdateCategoryDescriptionUI(int32 CategoryIndex);

	void FocusCategory();
	void FocusInventory(int32 PreferIndex = 0);

	void HandleMoveLeft();
	void HandleMoveRight();
	void HandleMoveUp();
	void HandleMoveDown();
	void HandleMoveInternal(ENavDir Dir);

	void HandleSelect();
	bool TryEquipSelectedItem();
	void HandleEquipSlotChanged(FGameplayTag Channel, const FKRUIMessage_EquipSlot& Msg);
	void HandleConfirmResult(FGameplayTag Channel, const FKRUIMessage_Confirm& Msg);

	void ShowInventorySlot();
	void HideInventorySlot();


	UFUNCTION() void OnCategorySelected(int32 Index, UKRItemSlotBase* SlotBase);
	UFUNCTION() void OnCategoryHovered(int32 Index, UKRItemSlotBase* SlotBase);
	UFUNCTION() void OnCategoryClicked();
	UFUNCTION() void OnInventorySelected(int32 Index, UKRItemSlotBase* SlotBase);
	UFUNCTION() void OnInventoryHovered(int32 Index, UKRItemSlotBase* SlotBase);
	UFUNCTION() void OnInventoryClicked();

	int32 StepGrid(int32 Cur, ENavDir Dir, int32 Cols, int32 Num) const;
	bool MoveCategory(ENavDir Dir);
	bool MoveInventory(ENavDir Dir);

	void HighlightEquippedItemInInventory(bool bSelect);
	
	bool IsCategorySlotEmpty(int32 Index) const;
	int32 FindFirstNonEmptySlot() const;
	int32 FindNextNonEmptySlot(int32 Current, ENavDir Dir) const;
};
