// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "KRShopMain.generated.h"

class UCommonButtonBase;
class UCommonButtonGroupBase;

UCLASS()
class KORAPROJECT_API UKRShopMain : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UCommonButtonBase> BuyButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UCommonButtonBase> SellButton;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

private:
	void HandleNext();
	void HandlePrev();
	void HandleSelect();

	void SetHoveredIndex(int32 NewIndex);

	UFUNCTION() void OnBuyClicked();
	UFUNCTION() void OnSellClicked();

	UFUNCTION() void HandleButtonHovered(UCommonButtonBase* Button, int32 Index);
	UFUNCTION() void HandleButtonSelected(UCommonButtonBase* Button, int32 Index);

	UPROPERTY()
	TObjectPtr<UCommonButtonGroupBase> ButtonGroup;

	UPROPERTY()
	TArray<TObjectPtr<UCommonButtonBase>> Buttons;

	int32 CurrentButtonIndex = 0;
};
