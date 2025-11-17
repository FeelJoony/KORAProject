// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "KRHUD.generated.h"

class UKRPlayerStatusWidget;
class UKRWeaponSwitchWidget;
class UKRBossHealthWidget;
class UKRQuestPanelWidget;
class UCommonUserWidget;

UCLASS()
class KORAPROJECT_API UKRHUD : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void InitializeHUD();

protected:
	UPROPERTY(meta = (BindWidget)) UKRPlayerStatusWidget* PlayerStatusBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRWeaponSwitchWidget* WeaponSwitch = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRBossHealthWidget* BossHP = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRQuestPanelWidget* QuestPanel = nullptr;
	UPROPERTY(meta = (BindWidget)) UCommonUserWidget* CurrencyPanel = nullptr;
};
