// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KROnlyGearingPanel.generated.h"

class UCommonNumericTextBlock;
class UHorizontalBox;
class UKRCurrencyComponent;

UCLASS()
class KORAPROJECT_API UKROnlyGearingPanel : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	void OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message);

	void UpdateGearingPanel(int32 InCurrentGearing, int32 InLostGearing);

	UPROPERTY(meta = (BindWidget)) UCommonNumericTextBlock* CurrentGearing = nullptr;
	UPROPERTY(meta = (BindWidget)) UHorizontalBox* LostGearingSection = nullptr;
	UPROPERTY(meta = (BindWidget)) UCommonNumericTextBlock* LostGearing = nullptr;

private:
	void RefreshFromCurrencyComponent();
	FGameplayMessageListenerHandle CurrencyListener;
};
