// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Currency/KRAllCurrencyPanel.h"
#include "Player/KRPlayerState.h"
#include "Components/KRCurrencyComponent.h"
#include "GameplayTag/KRShopTag.h"
#include "CommonNumericTextBlock.h"


void UKRAllCurrencyPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		CurrencyListener = Subsys.RegisterListener(
			FKRUIMessageTags::Currency(),
			this,
			&ThisClass::OnCurrencyMessageReceived
		);
	}

	RefreshFromCurrencyComponent();
}

void UKRAllCurrencyPanel::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		if (CurrencyListener.IsValid())
		{
			Subsys.UnregisterListener(CurrencyListener);
		}
	}

	Super::NativeDestruct();
}

void UKRAllCurrencyPanel::OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message)
{
	UpdateCurrencyPanel(Message.CurrentGearing, Message.CurrentCorbyte);
}

void UKRAllCurrencyPanel::UpdateCurrencyPanel(int32 InCurrentGearing, int32 InCurrentCorbyte)
{
	if (Gearing)
	{
		Gearing->SetText(FText::AsNumber(InCurrentGearing));
	}

	if (Corbyte)
	{
		Corbyte->SetText(FText::AsNumber(InCurrentCorbyte));
	}
}

void UKRAllCurrencyPanel::RefreshFromCurrencyComponent()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AKRPlayerState* KRPS = PC->GetPlayerState<AKRPlayerState>())
		{
			if (UKRCurrencyComponent* CurrencyComp = KRPS->GetCurrencyComponentSet())
			{
				const int32 CurrentGearing = CurrencyComp->GetCurrency(KRTAG_CURRENCY_PURCHASE_GEARING);
				const int32 CurrentCorbyte = CurrencyComp->GetCurrency(KRTAG_CURRENCY_SKILL_CORBYTE);

				UpdateCurrencyPanel(CurrentGearing, CurrentCorbyte);
			}
		}
	}
}
