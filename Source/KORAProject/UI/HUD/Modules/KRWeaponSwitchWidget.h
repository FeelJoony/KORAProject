// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "KRWeaponSwitchWidget.generated.h"


UCLASS()
class KORAPROJECT_API UKRWeaponSwitchWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void OnHUDInitialized() override;
	UFUNCTION(BlueprintCallable) void RefreshAll();

};
