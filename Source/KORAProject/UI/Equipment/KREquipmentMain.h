// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/KRCommonUIWidget.h"
#include "GameplayTagContainer.h"
#include "UI/Data/KRItemUIData.h" //전방선언 오류나서 수정했어요
#include "KREquipmentMain.generated.h"

class UUniformGridPanel;
class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UCommonButtonBase;
// �κ��丮 ������Ʈ

UCLASS()
class KORAPROJECT_API UKREquipmentMain : public UKRCommonUIWidget
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
	UPROPERTY(EditDefaultsOnly, Category = "UI|Input") FName Row_Back = "Back";
	UPROPERTY(EditDefaultsOnly, Category = "UI|Input") FName Row_Select = "Select";
	UPROPERTY(EditDefaultsOnly, Category = "UI|Input") FName Row_Next = "Next";
	UPROPERTY(EditDefaultsOnly, Category = "UI|Input") FName Row_Prev = "Prev";

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void SetupInputBindings() override;

private:
	//TWeakObjectPtr<UKRInventoryComponent> InventoryComp;
	TArray<FKRItemUIData> CachedUIData;

	UFUNCTION() void OnClickSwordModule();
	UFUNCTION() void OnClickSwordSkin();
	UFUNCTION() void OnClickGunModule();
	UFUNCTION() void OnClickGunSkin();
	UFUNCTION() void OnClickHeadColor();
	UFUNCTION() void OnClickDressColor();

	UFUNCTION() void OnGridSlotSelected(int32 CellIndex);

	void FilterAndCacheItems(const FGameplayTag& FilterTag);
	void RebuildInventoryUI(const TArray<FGameplayTag>& TagsAny);

	//void BindInventoryEvents();
	void UpdateDescriptionUI(int32 CellIndex);
};
