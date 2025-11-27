// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRShopBuy.generated.h"

class UKRSlotGridBase;
class UKRItemDescriptionBase;
class UCommonUserWidget;
struct FKRItemUIData;

UCLASS()
class KORAPROJECT_API UKRShopBuy : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UKRSlotGridBase> ShoppingSlot = nullptr;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UKRItemDescriptionBase> ShopItemDescription = nullptr;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) TObjectPtr<UCommonUserWidget> PlayerCurrencyPanel = nullptr;


	UFUNCTION(BlueprintCallable, Category = "Shop") void RefreshShopInventory();
	UFUNCTION(BlueprintCallable, Category = "Shop") void UpdatePlayerCurrency();

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnShopItemSelected(int32 SelectedIndex);
	void UpdateItemDescription(int32 CellIndex);

	void HandleSelect();

	void HandleMoveLeft();
	void HandleMoveRight();
	void HandleMoveUp();
	void HandleMoveDown();
	int32 StepGrid(int32 Current, int32 DirIndex, int32 NumColumns, int32 NumTotal) const;

private:
	UPROPERTY() TArray<FKRItemUIData> CachedShopItems;
	FGameplayMessageListenerHandle CurrencyListener;

	void OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message);
};
