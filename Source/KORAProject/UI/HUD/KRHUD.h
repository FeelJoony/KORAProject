// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "KRHUD.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
//class UCommonActivatableWidgetQueue; Only for Tutorial Widgets

UCLASS()
class KORAPROJECT_API UKRHUD : public UCommonUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonActivatableWidgetStack> HUDMainWidgetStack;
	/*UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UCommonActivatableWidgetQueue> TutorialQueue;*/

#pragma region WidgetsOnStack
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_PauseMenu;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_Inventory;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_Equipment;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_Settings;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_ShopMain;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_ShopSell;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_ShopBuy;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_WeaponUpgradeMain;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_InterestingTopics;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_InterestingConversation;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_WeaponUpgradePage;
	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_Quest;

	UPROPERTY(EditAnywhere, Category = "UIRouter") TSubclassOf<UCommonActivatableWidget> WBP_Confirm; // Modal
#pragma endregion

	virtual void NativeOnInitialized() override;


};
