// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/KREnemyHPWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UKREnemyHPWidget::InitFromASC(UAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	UnbindFromASC();

	if (!InASC)
	{
		ASCWeak.Reset();
		CombatAttrWeak.Reset();
		return;
	}

	ASCWeak = InASC;
	OwnerActorWeak = InOwnerActor;

	BindToASC(InASC);
	InitBarsFromASC();

    if (DamageAmount) DamageAmount->SetText(FText::FromString(TEXT("")));
}

void UKREnemyHPWidget::NativeDestruct()
{
	UnbindFromASC();
	Super::NativeDestruct();
}

void UKREnemyHPWidget::UnbindFromASC()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GreyDelayTimerHandle);
		World->GetTimerManager().ClearTimer(GreyAnimTimerHandle);
	}

	if (ASCWeak.IsValid() && HealthChangedHandle.IsValid())
	{
		ASCWeak->GetGameplayAttributeValueChangeDelegate(
			UKRCombatCommonSet::GetCurrentHealthAttribute()
		).Remove(HealthChangedHandle);

		HealthChangedHandle.Reset();
	}

	CombatAttrWeak.Reset();
}


void UKREnemyHPWidget::BindToASC(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	if (const UKRCombatCommonSet* Combat = InASC->GetSet<UKRCombatCommonSet>())
	{
		CombatAttrWeak = Combat;

		HealthChangedHandle =
			InASC->GetGameplayAttributeValueChangeDelegate(
				UKRCombatCommonSet::GetCurrentHealthAttribute()
			).AddUObject(this, &ThisClass::OnHealthAttributeChanged);
	}
}

void UKREnemyHPWidget::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!CombatAttrWeak.IsValid()) return;
    if (!MainHPBar || !GreyHPBar)  return;

    const float MaxHP = CombatAttrWeak->GetMaxHealth();
    if (MaxHP <= 0.f) return;

    const float OldHP = Data.OldValue;
    const float NewHP = Data.NewValue;

    const float OldPercent = FMath::Clamp(OldHP / MaxHP, 0.f, 1.f);
    const float NewPercent = FMath::Clamp(NewHP / MaxHP, 0.f, 1.f);
    const float Delta = NewPercent - OldPercent;

    MainHPBar->SetPercent(NewPercent);
    MainDisplayPercent = NewPercent;
    MainTargetPercent = NewPercent;

    if (DamageAmount && Delta < 0.f)
    {
        const float DamageValue = -(NewHP - OldHP);
        if (DamageValue > 0.f)
        {
            DamageAmount->SetText(FText::AsNumber(FMath::RoundToInt(DamageValue)));
        }
        else
        {
            DamageAmount->SetText(FText::FromString(TEXT("")));
        }
        BP_OnTakeDamage(DamageValue);
    }

    if (Delta < 0.f)
    {
        GreyHPBar->SetPercent(OldPercent);
        GreyDisplayPercent = OldPercent;
        GreyTargetPercent = NewPercent;

        if (UWorld* World = GetWorld())
        {
            auto& TM = World->GetTimerManager();
            TM.ClearTimer(GreyDelayTimerHandle);
            TM.ClearTimer(GreyAnimTimerHandle);
        }

        bGreyAnimating = false;
        bGreyPending = true;
        GreyElapsed = 0.f;

        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(
                GreyDelayTimerHandle,
                this,
                &ThisClass::StartGreyAnim,
                GreyDelay,
                false
            );
        }
    }
    else if (Delta > 0.f)
    {
        const float GreyCur = GreyHPBar->GetPercent();
        if (NewPercent >= GreyCur)
        {
            GreyHPBar->SetPercent(NewPercent);
            GreyDisplayPercent = GreyTargetPercent = NewPercent;
        }
    }
}

void UKREnemyHPWidget::InitBarsFromASC()
{
	if (!CombatAttrWeak.IsValid()) return;

	const float MaxHP = CombatAttrWeak->GetMaxHealth();
	const float CurHP = CombatAttrWeak->GetCurrentHealth();

	const float Percent = (MaxHP > 0.f)
		? FMath::Clamp(CurHP / MaxHP, 0.f, 1.f)
		: 0.f;

	if (MainHPBar)  MainHPBar->SetPercent(Percent);
	if (GreyHPBar)  GreyHPBar->SetPercent(Percent);

	MainDisplayPercent = MainTargetPercent = Percent;
	GreyDisplayPercent = GreyTargetPercent = Percent;
}

void UKREnemyHPWidget::StartGreyAnim()
{
    if (!GreyHPBar) return;

    bGreyPending = false;
    bGreyAnimating = true;
    GreyElapsed = 0.f;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(GreyAnimTimerHandle);
        World->GetTimerManager().SetTimer(
            GreyAnimTimerHandle,
            this,
            &ThisClass::TickGreyAnim,
            0.016f,  // 60fps (0.01f에서 최적화)
            true
        );
    }
}

void UKREnemyHPWidget::TickGreyAnim()
{
    if (!GreyHPBar) return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    const float DeltaTime = World->GetDeltaSeconds();
    GreyElapsed += DeltaTime;

    const float Alpha = (GreyAnimTime > 0.f)
        ? FMath::Clamp(GreyElapsed / GreyAnimTime, 0.f, 1.f)
        : 1.f;

    const float NewPercent = FMath::Lerp(GreyDisplayPercent, GreyTargetPercent, Alpha);

    // Slate Invalidation 최소화: 값이 변경될 때만 업데이트
    const float CurrentPercent = GreyHPBar->GetPercent();
    if (!FMath::IsNearlyEqual(CurrentPercent, NewPercent, 0.001f))
    {
        TickGreyHPBar(NewPercent);
    }

    if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
    {
        bGreyAnimating = false;
        GreyDisplayPercent = GreyTargetPercent;

        World->GetTimerManager().ClearTimer(GreyAnimTimerHandle);
    }
}

void UKREnemyHPWidget::TickGreyHPBar(float NewValueNormalized)
{
	if (!GreyHPBar) return;
	GreyHPBar->SetPercent(FMath::Clamp(NewValueNormalized, 0.f, 1.f));
}
