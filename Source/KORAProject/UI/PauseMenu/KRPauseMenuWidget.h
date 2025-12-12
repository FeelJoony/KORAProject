// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "UI/PauseMenu/KRSlotNameWidget.h"
#include "UI/PauseMenu/KRMenuTabButton.h"
#include "UI/HUD/Modules/KRQuickSlotWidget.h"
#include "KRPauseMenuWidget.generated.h"

class UKRInventorySubsystem;
class UKRSlotGridBase;
struct FKRItemUIData;

UCLASS()
class KORAPROJECT_API UKRPauseMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UKRMenuTabButton> EquipmentButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UKRMenuTabButton> InventoryButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UKRMenuTabButton> SkillTreeButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UKRMenuTabButton> SettingsButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UKRMenuTabButton> QuitButton;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UKRSlotNameWidget> SlotNameWidget;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UKRQuickSlotWidget> QuickSlotWidget;

	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UKRSlotGridBase> QuickSlotInventoryGrid;

private:
	void BindMenuButton(UKRMenuTabButton* Button);
	UFUNCTION() void HandleMenuHovered(UKRMenuTabButton* Button);
	UFUNCTION() void HandleQuickSlotHovered(FGameplayTag SlotDir);

	UFUNCTION() void HandleSlotNamePrimary(EKRSlotNameContext Context);
	UFUNCTION() void HandleSlotNameSecondary(EKRSlotNameContext Context);

	bool GetQuickItemUIData(FGameplayTag SlotDir, FKRItemUIData& OutData) const;
	void OpenQuickSlotInventoryForSlot(const FGameplayTag& SlotDir);
	void CloseQuickSlotInventory();
	void HandleQuickSlotInventorySelect();
	void HandleQuickSlotInventoryMove(uint8 DirIdx);
	int32 StepGridIndex(int32 Cur, uint8 DirIdx, int32 Cols, int32 Num) const;

	UFUNCTION() void HandleSelect();
	void HandleMoveLeft();
	void HandleMoveRight();
	void HandleMoveUp();
	void HandleMoveDown();

	EKRSlotNameContext CurrentSlotContext = EKRSlotNameContext::Menu;
	FName CurrentMenuRouteName;
	FGameplayTag CurrentQuickSlotDir;

	bool bQuickSlotInventoryOpen = false;
	TArray<FKRItemUIData> QuickSlotInventoryItemList; // From Inventory
};
