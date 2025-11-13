// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "KRPlayerStatusWidget.generated.h"

class UProgressBar; 
class UCommonNumericTextBlock; 
class UKRAttributeSet;

UCLASS()
class KORAPROJECT_API UKRPlayerStatusWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget)) UProgressBar* MainHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* GreyHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* StaminaBar = nullptr;

	virtual void OnHUDInitialized() override;
	virtual void NativeDestruct() override;
	virtual void UnbindAll() override;

private:
	void RefreshAll(const UKRAttributeSet* InAS) const;
	void OnAttrChanged();

};
