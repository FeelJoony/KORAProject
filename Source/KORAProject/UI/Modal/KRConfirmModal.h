// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRConfirmModal.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;

UCLASS()
class KORAPROJECT_API UKRConfirmModal : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetupConfirm(const FText& InMessage,
		EConfirmContext InContext,
		FGameplayTag InItemTag = FGameplayTag());

	UFUNCTION(BlueprintCallable)
	void SetupConfirmWithQuantity(const FText& InMessage,
		EConfirmContext InContext,
		FGameplayTag InItemTag,
		int32 InMin, int32 InMax, int32 InDefault);

	UFUNCTION(BlueprintCallable)
	void SetupConfirmWithQuickSlotTag(const FText& InMessage,
		EConfirmContext InContext,
		FGameplayTag InItemTag, FGameplayTag InSlotTag);

protected:
	UPROPERTY(meta = (BindWidget)) UCommonTextBlock* AlertText;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* YesButton;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* NoButton;

	UPROPERTY(meta = (BindWidgetOptional)) UCommonButtonBase* IncreaseButton;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonButtonBase* DecreaseButton;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonTextBlock* QuantityText;
	UPROPERTY(meta = (BindWidgetOptional)) UPanelWidget* QuantityPanel;

	virtual void NativeOnInitialized() override;
	virtual void NativeOnDeactivated() override;

private:
	EConfirmContext Context = EConfirmContext::Generic;
	FGameplayTag ItemTag;
	FGameplayTag QuickSlotTag;
	int32 MinQuantity = 0;
	int32 MaxQuantity = 0;
	int32 CurrentQuantity = 0;
	bool bUseQuantity = false;
	
	UFUNCTION() void HandleYes();
	UFUNCTION() void HandleNo();
	UFUNCTION() void HandleIncrease();
	UFUNCTION() void HandleDecrease();

	void RefreshQuantityUI();
};
