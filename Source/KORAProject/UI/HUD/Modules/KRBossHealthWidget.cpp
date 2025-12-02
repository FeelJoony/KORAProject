// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRBossHealthWidget.h"

#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UKRBossHealthWidget::SetBossASC(UAbilitySystemComponent* InASC)
{
	BossASCWeak = InASC;
	// Boss Name will be updated based on Boss Entity data in the future
	// Boss Name / Boss HPBar Visibility will be handled in Boss Encounter system
}

void UKRBossHealthWidget::OnHUDInitialized()
{
	Super::OnHUDInitialized();

	SetVisibility(ESlateVisibility::Collapsed);
	bBossHPBarVisible = false;

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		BossListenerHandle = Subsys.RegisterListener(
			FKRUIMessageTags::ProgressBar(),
			this,
			&ThisClass::OnBossMessage
		);
	}
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
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(BossListenerHandle);

		World->GetTimerManager().ClearTimer(BossAnimTimerHandle);
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UKRBossHealthWidget::OnBossMessage(FGameplayTag ChannelTag, const FKRUIMessage_Progress& Message)
{
	if (!IsMessageFromBoss(Message.TargetActor)) return;

	if (!bBossHPBarVisible)
	{
		SetVisibility(ESlateVisibility::Visible);
		bBossHPBarVisible = true;
	}

	if (Message.BarType != EProgressBarType::MainHP) return;

	const float Percent = (Message.MaxValue > 0.f) ? (Message.NewValue / Message.MaxValue) : 0.f;

	BossTargetPercent = Percent;
	StartBossAnim();
}

bool UKRBossHealthWidget::IsMessageFromBoss(const TWeakObjectPtr<AActor>& TargetActor) const
{
	if (!TargetActor.IsValid()) return false;
	if (!BossASCWeak.IsValid()) return false;

	const AActor* BossOwner = BossASCWeak->GetOwner();
	return BossOwner && TargetActor.Get() == BossOwner;
}

void UKRBossHealthWidget::StartBossAnim()
{
	if (!BossHP) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BossAnimTimerHandle);

		BossDisplayPercent = BossHP->Percent;
		BossAnimElapsed = 0.f;

		World->GetTimerManager().SetTimer(
			BossAnimTimerHandle,
			this,
			&ThisClass::TickBossAnim,
			0.01f,
			true
		);
	}
}

void UKRBossHealthWidget::TickBossAnim()
{
	if (!BossHP)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(BossAnimTimerHandle);
		}
		return;
	}

	if (UWorld* World = GetWorld())
	{
		BossAnimElapsed += World->GetDeltaSeconds();

		const float Alpha = (BossAnimTime > 0.f) ? FMath::Clamp(BossAnimElapsed / BossAnimTime, 0.f, 1.f) : 1.f;

		const float NewPercent = FMath::Lerp(BossDisplayPercent, BossTargetPercent, Alpha);
		BossHP->SetPercent(NewPercent);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(BossAnimTimerHandle);
		}
	}
}
