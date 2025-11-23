// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KRHUD.h"

#include "UI/HUD/Modules/KRPlayerStatusWidget.h"
#include "UI/HUD/Modules/KRBossHealthWidget.h"
#include "UI/HUD/Modules/KRQuestPanelWidget.h"

#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "Player/KRPlayerState.h"	

void UKRHUD::NativeOnActivated()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AKRPlayerState* PS = PC->GetPlayerState<AKRPlayerState>())
		{
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

			FKRHUDInitArgs Args;
			Args.ASC = ASC;

			InitializeChildrenHUD(Args);
		}
	}
}

void UKRHUD::NativeOnDeactivated()
{
	TearDownChildrenHUD();
	Super::NativeDestruct();
}

void UKRHUD::NativeDestruct()
{
	TearDownChildrenHUD();
	Super::NativeDestruct();
}

void UKRHUD::InitializeChildrenHUD(const FKRHUDInitArgs& Args)
{
	auto InitChild = [&Args](UObject* Widget)
		{
			if (Widget && Widget->Implements<UKRHUDWidgetInterface>())
			{
				IKRHUDWidgetInterface::Execute_InitializeHUD(Widget, Args);
			}
		};

	InitChild(PlayerStatusBar);
	InitChild(BossHPBar);
	InitChild(QuestPanel);
	InitChild(CurrencyPanel);

}

void UKRHUD::TearDownChildrenHUD()
{
	auto TearDownChild = [](UObject* Widget)
		{
			if (Widget && Widget->Implements<UKRHUDWidgetInterface>())
			{
				IKRHUDWidgetInterface::Execute_TearDownHUD(Widget);
			}
		};

	TearDownChild(PlayerStatusBar);
	TearDownChild(BossHPBar);
	TearDownChild(QuestPanel);
	TearDownChild(CurrencyPanel);
}


