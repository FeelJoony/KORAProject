// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Currency/KROnlyGearingPanel.h"
#include "CommonNumericTextBlock.h"
#include "Components/HorizontalBox.h"
#include "Math/UnrealMathUtility.h"

void UKROnlyGearingPanel::NativeConstruct()
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

void UKROnlyGearingPanel::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(CurrencyListener);
	}

	Super::NativeDestruct();
}

void UKROnlyGearingPanel::OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message)
{
	UpdateGearingPanel(Message.CurrentGearing, Message.DeltaGearing);
}

void UKROnlyGearingPanel::UpdateGearingPanel(int32 InCurrentGearing, int32 InLostGearing)
{
	if (CurrentGearing)
	{
		CurrentGearing->SetCurrentValue(InCurrentGearing);
	}

	if (LostGearing)
	{
		LostGearing->SetCurrentValue(FMath::Abs(InLostGearing));
	}

	if (LostGearingSection)
	{
		if (InLostGearing < 0)
		{
			LostGearingSection->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			LostGearingSection->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
