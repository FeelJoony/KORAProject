#include "Components/KRGuardRegainComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KREventTag.h"

UKRGuardRegainComponent::UKRGuardRegainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UKRGuardRegainComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기값 캐싱
	LastGreyHPValue = GetCurrentGreyHP();

	// 이벤트 리스너 설정 (ASC가 준비된 후)
	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&UKRGuardRegainComponent::SetupEventListeners,
			0.1f,
			false
		);
	}
}

void UKRGuardRegainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// GreyHP 자연 감소 처리
	ApplyGreyHPDecay(DeltaTime);

	// GreyHP 변경 감지 및 이벤트
	const float CurrentGreyHP = GetCurrentGreyHP();
	if (!FMath::IsNearlyEqual(LastGreyHPValue, CurrentGreyHP))
	{
		LastGreyHPValue = CurrentGreyHP;
		OnGreyHPChanged.Broadcast(CurrentGreyHP, GetMaxGreyHP());

		// GreyHP가 생겼으면 Decay 딜레이 리셋
		if (CurrentGreyHP > 0.f)
		{
			DecayDelayRemaining = GreyHPDecayDelay;
			bDecayActive = true;
		}
	}
}

void UKRGuardRegainComponent::SetupEventListeners()
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	// 공격 히트 이벤트 리스닝 (GreyHP → HP 회복)
	OnHitDelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(KRTAG_EVENT_COREDRIVE_CHARGE_ONHIT)
		.AddUObject(this, &UKRGuardRegainComponent::OnDamageDealtEvent);

	// 피격 이벤트 리스닝 (GreyHP 소멸)
	OnDamageTakenDelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(KRTAG_EVENT_COMBAT_HITREACTION)
		.AddUObject(this, &UKRGuardRegainComponent::OnDamageTakenEvent);
}

// ─────────────────────────────────────────────────────
// 조회 API
// ─────────────────────────────────────────────────────

float UKRGuardRegainComponent::GetCurrentGreyHP() const
{
	const UKRPlayerAttributeSet* AttrSet = GetPlayerAttributeSet();
	return AttrSet ? AttrSet->GetGreyHP() : 0.f;
}

float UKRGuardRegainComponent::GetMaxGreyHP() const
{
	// MaxGreyHP는 MaxHealth와 동일하게 취급
	const UKRCombatCommonSet* CombatSet = GetCombatCommonSet();
	return CombatSet ? CombatSet->GetMaxHealth() : 100.f;
}

bool UKRGuardRegainComponent::HasGreyHP() const
{
	return GetCurrentGreyHP() > 0.f;
}

// ─────────────────────────────────────────────────────
// 회복 API
// ─────────────────────────────────────────────────────

void UKRGuardRegainComponent::RecoverGreyHPOnAttack(float DamageDealt)
{
	if (DamageDealt <= 0.f) return;
	if (!HasGreyHP()) return;

	const float CurrentGreyHP = GetCurrentGreyHP();

	// 회복량 = 가한 데미지 × 회복률, 단 현재 GreyHP 이하
	const float RecoveryAmount = FMath::Min(CurrentGreyHP, DamageDealt * GreyHPRecoveryRate);

	if (RecoveryAmount > 0.f)
	{
		// GreyHP 감소
		SetGreyHP(CurrentGreyHP - RecoveryAmount);

		// HP 회복
		RecoverHealth(RecoveryAmount);

		// 이벤트 브로드캐스트
		OnGreyHPRecovered.Broadcast(RecoveryAmount);

		// Decay 딜레이 리셋 (공격 시에도 딜레이 리셋)
		DecayDelayRemaining = GreyHPDecayDelay;

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[GuardRegain] Recovered %.1f HP from GreyHP (Damage: %.1f, Rate: %.2f)"),
			RecoveryAmount, DamageDealt, GreyHPRecoveryRate);
#endif
	}
}

void UKRGuardRegainComponent::ClearAllGreyHP()
{
	const float CurrentGreyHP = GetCurrentGreyHP();
	if (CurrentGreyHP > 0.f)
	{
		SetGreyHP(0.f);
		bDecayActive = false;

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[GuardRegain] Cleared all GreyHP (%.1f)"), CurrentGreyHP);
#endif
	}
}

// ─────────────────────────────────────────────────────
// 내부 함수
// ─────────────────────────────────────────────────────

void UKRGuardRegainComponent::ApplyGreyHPDecay(float DeltaTime)
{
	if (!bDecayActive) return;
	if (!HasGreyHP())
	{
		bDecayActive = false;
		return;
	}

	// Decay 딜레이 처리
	if (DecayDelayRemaining > 0.f)
	{
		DecayDelayRemaining -= DeltaTime;
		return;
	}

	// GreyHP 자연 감소
	const float CurrentGreyHP = GetCurrentGreyHP();
	const float DecayAmount = GreyHPDecayRate * DeltaTime;
	const float NewGreyHP = FMath::Max(0.f, CurrentGreyHP - DecayAmount);

	SetGreyHP(NewGreyHP);

	if (NewGreyHP <= 0.f)
	{
		bDecayActive = false;
	}
}

void UKRGuardRegainComponent::SetGreyHP(float NewValue)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	const float ClampedValue = FMath::Max(0.f, NewValue);
	ASC->SetNumericAttributeBase(UKRPlayerAttributeSet::GetGreyHPAttribute(), ClampedValue);
}

void UKRGuardRegainComponent::RecoverHealth(float Amount)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	const UKRCombatCommonSet* CombatSet = GetCombatCommonSet();
	if (!CombatSet) return;

	const float CurrentHealth = CombatSet->GetCurrentHealth();
	const float MaxHealth = CombatSet->GetMaxHealth();
	const float NewHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);

	ASC->SetNumericAttributeBase(UKRCombatCommonSet::GetCurrentHealthAttribute(), NewHealth);
}

UAbilitySystemComponent* UKRGuardRegainComponent::GetASC() const
{
	if (!CachedASC.IsValid())
	{
		CachedASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	}
	return CachedASC.Get();
}

const UKRPlayerAttributeSet* UKRGuardRegainComponent::GetPlayerAttributeSet() const
{
	if (!CachedPlayerAttributeSet.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetASC())
		{
			CachedPlayerAttributeSet = ASC->GetSet<UKRPlayerAttributeSet>();
		}
	}
	return CachedPlayerAttributeSet.Get();
}

const UKRCombatCommonSet* UKRGuardRegainComponent::GetCombatCommonSet() const
{
	if (!CachedCombatCommonSet.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetASC())
		{
			CachedCombatCommonSet = ASC->GetSet<UKRCombatCommonSet>();
		}
	}
	return CachedCombatCommonSet.Get();
}

// ─────────────────────────────────────────────────────
// 이벤트 핸들러
// ─────────────────────────────────────────────────────

void UKRGuardRegainComponent::OnDamageDealtEvent(const FGameplayEventData* EventData)
{
	if (!EventData) return;

	// EventMagnitude에 데미지가 담겨있음
	RecoverGreyHPOnAttack(EventData->EventMagnitude);
}

void UKRGuardRegainComponent::OnDamageTakenEvent(const FGameplayEventData* EventData)
{
	// 피격 시 GreyHP 즉시 소멸 (가드 중인 경우 제외)
	// 가드 중인지는 GA에서 이벤트를 안 보내는 식으로 처리
	// 또는 여기서 태그 체크

	UAbilitySystemComponent* ASC = GetASC();
	if (ASC && ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Acting.Guard"))))
	{
		// 가드 중에는 GreyHP 유지 (가드 피격은 GreyHP 소멸 안 함)
		return;
	}

	// 일반 피격: GreyHP 즉시 소멸
	ClearAllGreyHP();
}
