// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "UI/Data/KRItemUIData.h"
#include "KREquipmentMain.generated.h"

class UUniformGridPanel;
class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UCommonButtonBase;
class UCommonButtonGroupBase;

UCLASS()
class KORAPROJECT_API UKREquipmentMain : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* SwordModuleSelect;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* SwordSkinSelect;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* GunModuleSelect;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* GunSkinSelect;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* HeadColorSelect;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* DressColorSelect;

	UPROPERTY(meta = (BindWidget)) UKRSlotGridBase* InventorySlot = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRItemDescriptionBase* ModuleDescription = nullptr;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	TArray<FKRItemUIData> CachedUIData;

	enum class EFocusRegion : uint8 { Category, Grid };
	EFocusRegion FocusRegion = EFocusRegion::Category;

	UPROPERTY() TObjectPtr<UCommonButtonGroupBase> CategoryGroup;
	UPROPERTY() TArray<TObjectPtr<UCommonButtonBase>> CategoryOrder;
	int32 LastCategoryIndex = 0;

	UFUNCTION() void OnClickSwordModule();
	UFUNCTION() void OnClickSwordSkin();
	UFUNCTION() void OnClickGunModule();
	UFUNCTION() void OnClickGunSkin();
	UFUNCTION() void OnClickHeadColor();
	UFUNCTION() void OnClickDressColor();

	struct FNavAdj { int32 L = -1, R = -1, U = -1, D = -1; };
	TArray<FNavAdj> CategoryAdj;

	void BuildCategoryGroup();
	UFUNCTION() void HandleCategoryChanged(UCommonButtonBase* SelectedButton, int32 ButtonIndex);
	void FocusCategory();
	void FocusGrid(int32 PreferIndex = 0);

	UFUNCTION() void OnGridSlotSelected(int32 CellIndex);
	void FilterAndCacheItems(const FGameplayTag& FilterTag);
	void RebuildInventoryUI(const FGameplayTag& FilterTag);
	void UpdateDescriptionUI(int32 CellIndex);

	void HandleMoveLeft();
	void HandleMoveRight();
	void HandleMoveUp();
	void HandleMoveDown();
	void HandleMoveInternal(uint8 DirIdx); // 0:L,1:R,2:U,3:D

	void HandleSelect();
	bool TryEquipSelectedItem();

	int32 StepCategory(int32 Cur, uint8 DirIdx) const;                    // 0:L,1:R,2:U,3:D
	int32 StepGrid(int32 Cur, uint8 DirIdx, int32 Cols, int32 Num) const; // ∞¯≈Î Ω∫≈‹
	bool  MoveGrid(uint8 DirIdx);
};
