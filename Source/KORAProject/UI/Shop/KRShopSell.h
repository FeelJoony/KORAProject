// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRShopSell.generated.h"

class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UCommonButtonBase;
class UCommonUserWidget;
struct FKRItemUIData;

UCLASS()
class KORAPROJECT_API UKRShopSell : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonButtonBase> ConsumablesButton = nullptr;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonButtonBase> MaterialButton = nullptr;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UKRSlotGridBase> ShoppingSlot = nullptr;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UKRItemDescriptionBase> ShopItemDescription = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) TObjectPtr<UCommonUserWidget> PlayerCurrencyPanel = nullptr;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void FilterSellableItems(const FGameplayTag& FilterTag);
	void OnPlayerItemSelected(int32 SelectedIndex);
	void UpdateItemDescription(int32 CellIndex);

	void HandleSelect();

	void HandleMoveLeft();
	void HandleMoveRight();
	void HandleMoveUp();
	void HandleMoveDown();
	int32 StepGrid(int32 Current, int32 DirIndex, int32 NumColumns, int32 NumTotal) const;

	UFUNCTION(BlueprintCallable, Category = "Shop") void RefreshShopInventory();

	void OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message);

	void OnClickConsumables();
	void OnClickMaterial();

	UPROPERTY() TArray<FKRItemUIData> CachedShopItems;
	FGameplayTag CurrentFilterTag;
	FGameplayMessageListenerHandle CurrencyListener;
};
