// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "UI/HUD/KRHUDWidgetInterface.h"
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

protected:
	UPROPERTY(meta = (BindWidget)) UKRPlayerStatusWidget* PlayerStatusBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRBossHealthWidget* BossHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UKRQuestPanelWidget* QuestPanel = nullptr;
	UPROPERTY(meta = (BindWidget)) UCommonUserWidget* CurrencyPanel = nullptr;

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeDestruct() override;

private:
	void InitializeChildrenHUD(const FKRHUDInitArgs& Args);
	void TearDownChildrenHUD();
};
