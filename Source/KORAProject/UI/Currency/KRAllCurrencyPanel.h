// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRAllCurrencyPanel.generated.h"

class UCommonNumericTextBlock;
class UKRCurrencyComponent;

UCLASS()
class KORAPROJECT_API UKRAllCurrencyPanel : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	void OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message);

	void UpdateCurrencyPanel(int32 InCurrentGearing, int32 InCurrentCorbyte);

	UPROPERTY(meta = (BindWidget)) UCommonNumericTextBlock* Gearing = nullptr;
	UPROPERTY(meta = (BindWidget)) UCommonNumericTextBlock* Corbyte = nullptr;

private:
	void RefreshFromCurrencyComponent();

	FGameplayMessageListenerHandle CurrencyListener;
};
