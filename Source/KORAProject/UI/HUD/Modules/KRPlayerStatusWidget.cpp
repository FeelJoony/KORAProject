// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRPlayerStatusWidget.h"
//#include "GameFramework/AsyncAction_ListenForGameplayMessage.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

static FGameplayTag TAG_UI_Message_ProgressBar() { return FGameplayTag::RequestGameplayTag(TEXT("UI.Message.ProgressBar")); }
static FGameplayTag TAG_ProgressBar_Health() { return FGameplayTag::RequestGameplayTag(TEXT("UI.ProgressBar.HP")); }
static FGameplayTag TAG_ProgressBar_Stamina() { return FGameplayTag::RequestGameplayTag(TEXT("UI.ProgressBar.Stamina")); }
static FGameplayTag TAG_ProgressBar_CoreDrive() { return FGameplayTag::RequestGameplayTag(TEXT("UI.ProgressBar.CoreDrive")); }

static FTimerHandle GHealthAnimTimerHandle;
static FTimerHandle GStaminaAnimTimerHandle;

void UKRPlayerStatusWidget::OnHUDInitialized()
{
	Super::OnHUDInitialized();

	if (UWorld* World = GetWorld())
	{
		if (PBMessageListener.IsValid())
		{
			UGameplayMessageSubsystem::Get(World).UnregisterListener(PBMessageListener);
		}

		PBMessageListener = UGameplayMessageSubsystem::Get(World).RegisterListener<FKRProgressBarMessages>(
				TAG_UI_Message_ProgressBar(),
				this,
				&UKRPlayerStatusWidget::ProcessMessage
			);
	}
	
	HealthDisplayPercent = HealthTargetPercent = 1.f;
	StaminaDisplayPercent = StaminaTargetPercent = 1.f;

	HealthAnimElapsed = 0.f;
	StaminaAnimElapsed = 0.f;
}

void UKRPlayerStatusWidget::NativeDestruct()
{
	UnbindAll();
	Super::NativeDestruct();
}

void UKRPlayerStatusWidget::UnbindAll()
{
	if (PBMessageListener.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(PBMessageListener);
	}
}

void UKRPlayerStatusWidget::ProcessMessage(FGameplayTag InTag, const FKRProgressBarMessages& Message)
{
	if (!IsMessageFromMyActor(Message))
		return;

	if (Message.MaxValue <= 0.f)
		return;

	const float Percent = Message.NewValue / Message.MaxValue;

	if (Message.ProgressBarTag == TAG_ProgressBar_Health())
	{
		if (!MainHPBar)
			return;

		if (Message.Delta < 0.f) // Damage
		{
			if (GreyHPBar)
			{
				GreyHPBar->SetPercent(HealthDisplayPercent);
			}

			HealthTargetPercent = Percent;
			StartHealthAnim();
		}
		else // Recovery
		{
			HealthDisplayPercent = HealthTargetPercent = Percent;
			MainHPBar->SetPercent(Percent);
			if (GreyHPBar)
			{
				GreyHPBar->SetPercent(Percent);
			}

			if (UWorld* World = GetWorld())
			{
				if (HealthAnimTimerHandle)
				{
					World->GetTimerManager().ClearTimer(*HealthAnimTimerHandle);
				}
			}
		}
	}
	else if (Message.ProgressBarTag == TAG_ProgressBar_Stamina())
	{
		if (!StaminaBar)
			return;

		StaminaTargetPercent = Percent;
		StartStaminaAnim();
	}
	else if (Message.ProgressBarTag == TAG_ProgressBar_CoreDrive())
	{
		CoreDriveValue = Message.NewValue;
		CoreDriveMax = (Message.MaxValue > 0.f) ? Message.MaxValue : 60.f;

		const float Clamped = FMath::Clamp(CoreDriveValue, 0.f, CoreDriveMax);
		const float Unit = CoreDriveMax / 3.f;
		const int32 FullSeg = FMath::FloorToInt(Clamped / Unit);
		const float Rem = Clamped - FullSeg * Unit;
		const float Partial = (FullSeg < 3) ? (Rem / Unit) : 0.f;

		BP_OnCoreDriveChanged(FullSeg, Partial);
	}
}


bool UKRPlayerStatusWidget::IsMessageFromMyActor(const FKRProgressBarMessages& Message) const
{
	if (!ASCWeak.IsValid())
		return false;

	AActor* MyActor = ASCWeak->GetAvatarActor();
	return MyActor && Message.TargetActor.Get() == MyActor;
}

void UKRPlayerStatusWidget::StartHealthAnim()
{
	if (!HealthAnimTimerHandle || !GetWorld())
		return;

	HealthAnimElapsed = 0.f;
	GetWorld()->GetTimerManager().SetTimer(
		*HealthAnimTimerHandle,
		this,
		&UKRPlayerStatusWidget::TickHealthAnim,
		0.016f,
		true
	);
}

void UKRPlayerStatusWidget::TickHealthAnim()
{
	if (!GetWorld() || !MainHPBar)
		return;

	HealthAnimElapsed += 0.016f;
	const float Alpha = (HealthAnimTime > 0.f) ? FMath::Clamp(HealthAnimElapsed / HealthAnimTime, 0.f, 1.f) : 1.f;

	HealthDisplayPercent = FMath::Lerp(HealthDisplayPercent, HealthTargetPercent, Alpha);
	MainHPBar->SetPercent(HealthDisplayPercent);

	if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
	{
		HealthDisplayPercent = HealthTargetPercent;
		MainHPBar->SetPercent(HealthDisplayPercent);
		GetWorld()->GetTimerManager().ClearTimer(*HealthAnimTimerHandle);
	}
}

void UKRPlayerStatusWidget::StartStaminaAnim()
{
	if (!StaminaAnimTimerHandle || !GetWorld())
		return;

	StaminaAnimElapsed = 0.f;
	GetWorld()->GetTimerManager().SetTimer(
		*StaminaAnimTimerHandle,
		this,
		&UKRPlayerStatusWidget::TickStaminaAnim,
		0.016f,
		true
	);
}

void UKRPlayerStatusWidget::TickStaminaAnim()
{
	if (!GetWorld() || !StaminaBar)
		return;

	StaminaAnimElapsed += 0.016f;
	const float Alpha = (StaminaAnimTime > 0.f) ? FMath::Clamp(StaminaAnimElapsed / StaminaAnimTime, 0.f, 1.f) : 1.f;

	StaminaDisplayPercent = FMath::Lerp(StaminaDisplayPercent, StaminaTargetPercent, Alpha);
	StaminaBar->SetPercent(StaminaDisplayPercent);

	if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
	{
		StaminaDisplayPercent = StaminaTargetPercent;
		StaminaBar->SetPercent(StaminaDisplayPercent);
		GetWorld()->GetTimerManager().ClearTimer(*StaminaAnimTimerHandle);
	}
}