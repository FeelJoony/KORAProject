// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRBossHealthWidget.h"
#include "UI/Data/UIStruct/KRProgressBarMessages.h"
#include "GameplayTag/KRUITag.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UKRBossHealthWidget::SetBossASC(UAbilitySystemComponent* InASC)
{
	BossASCWeak = InASC;
}

void UKRBossHealthWidget::OnHUDInitialized()
{
	if (UWorld* World = GetWorld())
	{
		if (BossListenerHandle.IsValid())
		{
			UGameplayMessageSubsystem::Get(World).UnregisterListener(BossListenerHandle);
			BossListenerHandle = FGameplayMessageListenerHandle();
		}

		BossListenerHandle = UGameplayMessageSubsystem::Get(World).RegisterListener<FKRProgressBarMessages>(
				KRTAG_UI_MESSAGE_PROGRESSBAR,
				this,
				&UKRBossHealthWidget::OnBossMessage
		);
	}

	BossDisplayPercent = BossTargetPercent = 1.f;
	BossAnimElapsed = 0.f;
}

void UKRBossHealthWidget::NativeDestruct()
{
	UnbindAll();
	Super::NativeDestruct();
}

void UKRBossHealthWidget::UnbindAll()
{
	if (UWorld* World = GetWorld())
	{
		if (BossListenerHandle.IsValid())
		{
			UGameplayMessageSubsystem::Get(World).UnregisterListener(BossListenerHandle);
			BossListenerHandle = FGameplayMessageListenerHandle();
		}

		World->GetTimerManager().ClearTimer(BossAnimTimerHandle);
	}
}

void UKRBossHealthWidget::OnBossMessage(FGameplayTag ChannelTag, const FKRProgressBarMessages& Message)
{
	if (!IsMessageFromBoss(Message))
		return;

	if (Message.ProgressBarTag != KRTAG_UI_PROGRESSBAR_HP || Message.MaxValue <= 0.f)
		return;

	const float NewPercent = Message.NewValue / Message.MaxValue;
	BossTargetPercent = NewPercent;

	StartBossAnim();
}

bool UKRBossHealthWidget::IsMessageFromBoss(const FKRProgressBarMessages& Message) const
{
	const UAbilitySystemComponent* BossASC = BossASCWeak.Get();
	if (!BossASC)
		return false;

	AActor* BossAvatar = BossASC->GetAvatarActor();
	return BossAvatar && Message.TargetActor.Get() == BossAvatar;
}

void UKRBossHealthWidget::StartBossAnim()
{
	if (UWorld* World = GetWorld())
	{
		if (!BossHP) return;

		BossAnimElapsed = 0.f;
		World->GetTimerManager().SetTimer(BossAnimTimerHandle, this, &UKRBossHealthWidget::TickBossAnim, 0.016f, true);
	}
}

void UKRBossHealthWidget::TickBossAnim()
{
	if (!BossHP || GetWorld()) return;

	BossAnimElapsed += 0.016f;
	const float Alpha = (BossAnimTime > 0.f) ? FMath::Clamp(BossAnimElapsed / BossAnimTime, 0.f, 1.f) : 1.f;

	BossDisplayPercent = FMath::Lerp(BossDisplayPercent, BossTargetPercent, Alpha);
	BossHP->SetPercent(BossDisplayPercent);

	if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
	{
		BossDisplayPercent = BossTargetPercent;
		BossHP->SetPercent(BossDisplayPercent);
		GetWorld()->GetTimerManager().ClearTimer(BossAnimTimerHandle);
	}
}

