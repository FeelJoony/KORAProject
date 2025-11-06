// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KRHUD.h"
#include "SubSystem/KRUIRouterSubsystem.h"

#include "Widgets/CommonActivatableWidgetContainer.h"

void UKRHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* Router = GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("NativeOnInitialized,,, Register Routes..."));
		Router->RegisterStack(TEXT("Screen"), HUDMainWidgetStack);

		Router->RegisterRoute(TEXT("PauseMenu"), { TEXT("Screen"), WBP_PauseMenu, EKRUIGameStopPolicy::PauseWhileTop });
		Router->RegisterRoute(TEXT("Inventory"), { TEXT("Screen"), WBP_Inventory, EKRUIGameStopPolicy::PauseWhileTop });
		Router->RegisterRoute(TEXT("Equipment"), { TEXT("Screen"), WBP_Equipment, EKRUIGameStopPolicy::None });
		Router->RegisterRoute(TEXT("Settings"), { TEXT("Screen"), WBP_Settings, EKRUIGameStopPolicy::PauseWhileOpen });
		Router->RegisterRoute(TEXT("ShopMain"), { TEXT("Screen"), WBP_ShopMain, EKRUIGameStopPolicy::None });
		Router->RegisterRoute(TEXT("ShopSell"), { TEXT("Screen"), WBP_ShopSell, EKRUIGameStopPolicy::PauseWhileOpen });
		Router->RegisterRoute(TEXT("ShopBuy"), { TEXT("Screen"), WBP_ShopBuy, EKRUIGameStopPolicy::PauseWhileOpen });
		Router->RegisterRoute(TEXT("WeaponMain"), { TEXT("Screen"), WBP_WeaponUpgradeMain, EKRUIGameStopPolicy::PauseWhileTop });
		Router->RegisterRoute(TEXT("InterestingTopics"), { TEXT("Screen"), WBP_InterestingTopics, EKRUIGameStopPolicy::None });
		Router->RegisterRoute(TEXT("InterestingConversation"), { TEXT("Screen"), WBP_InterestingConversation, EKRUIGameStopPolicy::None });
		Router->RegisterRoute(TEXT("WeaponUpgrade"), { TEXT("Screen"), WBP_WeaponUpgradePage, EKRUIGameStopPolicy::PauseWhileOpen });
		Router->RegisterRoute(TEXT("Quest"), { TEXT("Screen"), WBP_Quest, EKRUIGameStopPolicy::None });
		Router->RegisterRoute(TEXT("Confirm"), { TEXT("Screen"), WBP_Confirm, EKRUIGameStopPolicy::PauseWhileOpen });
	}
}
