// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Currency/KRAllCurrencyPanel.h"
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
}

void UKRAllCurrencyPanel::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(CurrencyListener);
	}

	Super::NativeDestruct();
}

void UKRAllCurrencyPanel::OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message)
{
	UpdateCurrency(Message.CurrentGearing, Message.CurrentCorbyte);
}

void UKRAllCurrencyPanel::UpdateCurrency(int32 InGearing, int32 InCorbyte)
{
	if (Gearing)
	{
		Gearing->SetText(FText::AsNumber(InGearing));
	}

	if (Corbyte)
	{
		Corbyte->SetText(FText::AsNumber(InCorbyte));
	}
}
