// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRBossHealthWidget.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "Components/ProgressBar.h"
#include "CommonTextBlock.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"


void UKRBossHealthWidget::OnHUDInitialized()
{
	Super::OnHUDInitialized();

	if (BossHP)
	{
		BossHP->SetPercent(1.f);
		BossHP->SetVisibility(ESlateVisibility::Hidden);
	}
	SetVisibility(ESlateVisibility::Collapsed);

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);

		BossMessageListenerHandle = MessageSubsystem.RegisterListener<FKRUIMessage_Boss>(
			FKRUIMessageTags::Boss(), this, &ThisClass::OnBossMessageReceived);
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
		World->GetTimerManager().ClearTimer(BossAnimTimerHandle);

		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
		if (BossMessageListenerHandle.IsValid())
		{
			MessageSubsystem.UnregisterListener(BossMessageListenerHandle);
		}
	}

	if (BossASCWeak.IsValid() && BossHealthChangedHandle.IsValid())
	{
		BossASCWeak->GetGameplayAttributeValueChangeDelegate(
			UKRCombatCommonSet::GetCurrentHealthAttribute()
		).Remove(BossHealthChangedHandle);

		BossHealthChangedHandle.Reset();
	}

	BossASCWeak.Reset();
	BossCombatAttr.Reset();
}



void UKRBossHealthWidget::BindToBossASC(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	BossASCWeak = InASC;

	if (const UKRCombatCommonSet* Combat = InASC->GetSet<UKRCombatCommonSet>())
	{
		BossCombatAttr = Combat;

		BossHealthChangedHandle =
			InASC->GetGameplayAttributeValueChangeDelegate(
				UKRCombatCommonSet::GetCurrentHealthAttribute()
			).AddUObject(this, &ThisClass::OnBossHealthAttributeChanged);
	}
}

void UKRBossHealthWidget::InitBossBarFromASC()
{
	if (!BossCombatAttr.IsValid() || !BossHP) return;

	const float MaxHP = BossCombatAttr->GetMaxHealth();
	const float CurHP = BossCombatAttr->GetCurrentHealth();

	const float HPPercent = (MaxHP > 0.f)
		? FMath::Clamp(CurHP / MaxHP, 0.f, 1.f)
		: 0.f;

	BossHP->SetPercent(HPPercent);
	BossDisplayPercent = BossTargetPercent = HPPercent;
}

void UKRBossHealthWidget::OnBossHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!BossCombatAttr.IsValid() || !BossHP) return;

	const float MaxHP = BossCombatAttr->GetMaxHealth();
	if (MaxHP <= 0.f) return;

	const float OldHP = Data.OldValue;
	const float NewHP = Data.NewValue;

	if (NewHP <= 0.f)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const float OldPercent = FMath::Clamp(OldHP / MaxHP, 0.f, 1.f);
	const float NewPercent = FMath::Clamp(NewHP / MaxHP, 0.f, 1.f);
	const float Delta = NewPercent - OldPercent;

	if (Delta < 0.f)
	{
		BossAnimTime = 0.2f;
	}
	else
	{
		BossAnimTime = 0.3f;
	}

	BossDisplayPercent = BossHP->GetPercent();
	BossTargetPercent = NewPercent;

	StartBossAnim();
}

void UKRBossHealthWidget::StartBossAnim()
{
	if (!BossHP) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BossAnimTimerHandle);

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
	if (!BossHP) return;

	if (UWorld* World = GetWorld())
	{
		BossAnimElapsed += World->GetDeltaSeconds();
		const float Alpha = (BossAnimTime > 0.f)
			? FMath::Clamp(BossAnimElapsed / BossAnimTime, 0.f, 1.f)
			: 1.f;

		const float NewPercent = FMath::Lerp(BossDisplayPercent, BossTargetPercent, Alpha);
		BossHP->SetPercent(NewPercent);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(BossAnimTimerHandle);
		}
	}
}

void UKRBossHealthWidget::OnBossMessageReceived(FGameplayTag Channel, const FKRUIMessage_Boss& Message)
{
	if (Message.BossASC.IsValid())
	{
		SetBossASC(Message.BossASC.Get(), Message.BossNameKey);
	}
}

void UKRBossHealthWidget::SetBossASC(UAbilitySystemComponent* InASC, const FName& InBossNameKey)
{
	UnbindAll();

	if (!InASC) return;

	BindToBossASC(InASC);
	InitBossBarFromASC();

	static const FName TableId = FName("/Game/UI/StringTable/ST_EnemyNames");

	if (BossName)
	{
		FText BossDisplayName = FText::FromStringTable(TableId, InBossNameKey.ToString());
		BossName->SetText(BossDisplayName);
	}

	SetVisibility(ESlateVisibility::Visible);
}