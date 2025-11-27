// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRPlayerStatusWidget.h"

#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UKRPlayerStatusWidget::OnHUDInitialized()
{
	Super::OnHUDInitialized();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		ProgressMessageHandle = Subsys.RegisterListener(
			FKRUIMessageTags::ProgressBar(),
			this,
			&ThisClass::OnProgressMessageReceived
		);

		GreyHPMessageHandle = Subsys.RegisterListener(
			FKRUIMessageTags::GreyHP(),
			this,
			&ThisClass::OnGreyHPMessageReceived
		);

		WeaponMessageHandle = Subsys.RegisterListener(
			FKRUIMessageTags::Weapon(),
			this,
			&ThisClass::OnWeaponMessageReceived
		);
	}
}

void UKRPlayerStatusWidget::NativeDestruct()
{
	UnbindAll();
	Super::NativeDestruct();
}

void UKRPlayerStatusWidget::UnbindAll()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		Subsys.UnregisterListener(ProgressMessageHandle);
		Subsys.UnregisterListener(GreyHPMessageHandle);
		Subsys.UnregisterListener(WeaponMessageHandle);

		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(HealthAnimTimerHandle);
		TM.ClearTimer(StaminaAnimTimerHandle);
		TM.ClearTimer(GreyHPDecayTimerHandle);
	}
}

void UKRPlayerStatusWidget::OnProgressMessageReceived(FGameplayTag Channel, const FKRUIMessage_Progress& Message)
{
	if (!IsMessageFromMyActor(Message.TargetActor)) return;

	const float Percent = (Message.MaxValue > 0.f) ? (Message.NewValue / Message.MaxValue) : 0.f;

	switch (Message.BarType)
	{
	case EProgressBarType::MainHP:
		HealthTargetPercent = Percent;
		StartHealthAnim();
		break;

	case EProgressBarType::Stamina:
		StaminaTargetPercent = Percent;
		StartStaminaAnim();
		break;

	case EProgressBarType::CoreDrive:
		CoreDriveValue = Message.NewValue;
		CoreDriveMax = Message.MaxValue;

		if (CoreDriveMax > 0.f)
		{
			const float SegmentSize = 10.f;
			const int32 FullSegments = FMath::FloorToInt(CoreDriveValue / SegmentSize);
			const float Partial = FMath::Frac(CoreDriveValue / SegmentSize);
			BP_OnCoreDriveChanged(FullSegments, Partial);
		}
		break;

	case EProgressBarType::GreyHP:
		CurrentGreyHP = Percent;
		UpdateGreyHPBar(CurrentGreyHP);
		break;
	}
}

void UKRPlayerStatusWidget::OnGreyHPMessageReceived(FGameplayTag Channel, const FKRUIMessage_GreyHP& Message)
{
	if (Message.bRecovered)
	{
		RecoverGreyHP(Message.RecoveredAmount);
	}
	else
	{
		DecayGreyHP();
	}
}

void UKRPlayerStatusWidget::OnWeaponMessageReceived(FGameplayTag Channel, const FKRUIMessage_Weapon& Message)
{
	BP_OnWeaponChanged(Message);
}

bool UKRPlayerStatusWidget::IsMessageFromMyActor(const TWeakObjectPtr<AActor>& TargetActor) const
{
	if (!TargetActor.IsValid())
	{
		return false;
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APawn* MyPawn = PC->GetPawn())
		{
			return TargetActor.Get() == MyPawn;
		}
	}

	return false;
}

void UKRPlayerStatusWidget::StartHealthAnim()
{
	if (!MainHPBar) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HealthAnimTimerHandle);

		HealthDisplayPercent = MainHPBar->Percent;
		HealthAnimElapsed = 0.f;

		World->GetTimerManager().SetTimer(
			HealthAnimTimerHandle,
			this,
			&ThisClass::TickHealthAnim,
			0.01f,
			true
		);
	}
}

void UKRPlayerStatusWidget::TickHealthAnim()
{
	if (!MainHPBar) return;

	if (UWorld* World = GetWorld())
	{
		HealthAnimElapsed += World->GetDeltaSeconds();
		const float Alpha = (HealthAnimTime > 0.f) ? FMath::Clamp(HealthAnimElapsed / HealthAnimTime, 0.f, 1.f) : 1.f;

		const float NewPercent = FMath::Lerp(HealthDisplayPercent, HealthTargetPercent, Alpha);
		MainHPBar->SetPercent(NewPercent);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(HealthAnimTimerHandle);
		}
	}
}

void UKRPlayerStatusWidget::StartStaminaAnim()
{
	if (!StaminaBar) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StaminaAnimTimerHandle);

		StaminaDisplayPercent = StaminaBar->Percent;
		StaminaAnimElapsed = 0.f;

		World->GetTimerManager().SetTimer(
			StaminaAnimTimerHandle,
			this,
			&ThisClass::TickStaminaAnim,
			0.01f,
			true
		);
	}
}

void UKRPlayerStatusWidget::TickStaminaAnim()
{
	if (!StaminaBar) return;

	if (UWorld* World = GetWorld())
	{
		StaminaAnimElapsed += World->GetDeltaSeconds();
		const float Alpha = (StaminaAnimTime > 0.f) ? FMath::Clamp(StaminaAnimElapsed / StaminaAnimTime, 0.f, 1.f) : 1.f;

		const float NewPercent = FMath::Lerp(StaminaDisplayPercent, StaminaTargetPercent, Alpha);
		StaminaBar->SetPercent(NewPercent);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(StaminaAnimTimerHandle);
		}
	}
}

void UKRPlayerStatusWidget::RecoverGreyHP(float RecoveredAmount)
{
	CurrentGreyHP = FMath::Clamp(CurrentGreyHP - RecoveredAmount, 0.f, 1.f);
	UpdateGreyHPBar(CurrentGreyHP);

	if (CurrentGreyHP > KINDA_SMALL_NUMBER)
	{
		DecayGreyHP();
	}
	else
	{
		CurrentGreyHP = 0.f;
		UpdateGreyHPBar(0.f);

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(GreyHPDecayTimerHandle);
		}
	}
}

void UKRPlayerStatusWidget::DecayGreyHP()
{
	if (!GreyHPBar) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GreyHPDecayTimerHandle);

		GreyHPDecayStartValue = CurrentGreyHP;
		GreyHPDecayElapsed = 0.f;

		World->GetTimerManager().SetTimer(
			GreyHPDecayTimerHandle,
			this,
			&ThisClass::TickGreyHPDecayAnimation,
			0.02f,
			true
		);
	}
}

void UKRPlayerStatusWidget::TickGreyHPDecayAnimation()
{
	if (!GreyHPBar)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(GreyHPDecayTimerHandle);
		}
		return;
	}

	if (UWorld* World = GetWorld())
	{
		GreyHPDecayElapsed += World->GetDeltaSeconds();
		const float Alpha = (GreyHPDecayDuration > 0.f) ? FMath::Clamp(GreyHPDecayElapsed / GreyHPDecayDuration, 0.f, 1.f) : 1.f;

		const float NewValue = FMath::Lerp(GreyHPDecayStartValue, 0.f, Alpha);
		CurrentGreyHP = NewValue;
		UpdateGreyHPBar(CurrentGreyHP);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(GreyHPDecayTimerHandle);
			CurrentGreyHP = 0.f;
			UpdateGreyHPBar(0.f);
		}
	}
}

void UKRPlayerStatusWidget::UpdateGreyHPBar(float NewValueNormalized)
{
	if (GreyHPBar)
	{
		GreyHPBar->SetPercent(FMath::Clamp(NewValueNormalized, 0.f, 1.f));
	}
}
