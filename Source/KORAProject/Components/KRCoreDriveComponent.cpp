#include "Components/KRCoreDriveComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GameplayTag/KREventTag.h"

UKRCoreDriveComponent::UKRCoreDriveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKRCoreDriveComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기값 캐싱
	LastCoreDriveValue = GetCurrentCoreDrive();

	// 이벤트 리스너 설정 (ASC가 준비된 후)
	// 약간의 딜레이를 주어 ASC 초기화 완료 대기
	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&UKRCoreDriveComponent::SetupEventListeners,
			0.1f,
			false
		);
	}
}

void UKRCoreDriveComponent::SetupEventListeners()
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	// 공격 히트 이벤트 리스닝
	OnHitDelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(KRTAG_EVENT_COREDRIVE_CHARGE_ONHIT)
		.AddUObject(this, &UKRCoreDriveComponent::OnDamageDealtEvent);

	// 퍼펙트 가드 이벤트 리스닝
	OnParryDelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(KRTAG_EVENT_COREDRIVE_CHARGE_ONPARRY)
		.AddUObject(this, &UKRCoreDriveComponent::OnDamageDealtEvent);

	// 적 처치 이벤트 리스닝
	OnKillDelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(KRTAG_EVENT_COREDRIVE_CHARGE_ONKILL)
		.AddUObject(this, &UKRCoreDriveComponent::OnDamageDealtEvent);
}

// ─────────────────────────────────────────────────────
// 충전 API
// ─────────────────────────────────────────────────────

void UKRCoreDriveComponent::ChargeCoreDrive(float Amount)
{
	if (Amount <= 0.f) return;

	ApplyCoreDriveChange(Amount);

	// 풀 충전 체크
	const float CurrentPercent = GetCoreDrivePercent();
	if (CurrentPercent >= 1.0f && !bWasFullLastFrame)
	{
		bWasFullLastFrame = true;
		OnCoreDriveFull.Broadcast();

		// 풀 충전 이벤트 전송
		if (AActor* Owner = GetOwner())
		{
			FGameplayEventData EventData;
			EventData.EventTag = KRTAG_EVENT_COREDRIVE_FULL;
			EventData.Instigator = Owner;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, KRTAG_EVENT_COREDRIVE_FULL, EventData);
		}
	}
}

void UKRCoreDriveComponent::ChargeOnHit(float DamageDealt)
{
	const float ChargeAmount = DamageDealt * ChargeConfig.HitChargeMultiplier;
	ChargeCoreDrive(ChargeAmount);
}

void UKRCoreDriveComponent::ChargeOnPerfectGuard()
{
	ChargeCoreDrive(ChargeConfig.PerfectGuardChargeAmount);
}

void UKRCoreDriveComponent::ChargeOnKill()
{
	ChargeCoreDrive(ChargeConfig.KillChargeAmount);
}

// ─────────────────────────────────────────────────────
// 소모 API
// ─────────────────────────────────────────────────────

bool UKRCoreDriveComponent::ConsumeCoreDrive(float Amount)
{
	if (Amount <= 0.f) return true;

	if (!HasEnoughCoreDrive(Amount))
	{
		return false;
	}

	ApplyCoreDriveChange(-Amount);
	bWasFullLastFrame = false;
	return true;
}

bool UKRCoreDriveComponent::ConsumeAllCoreDrive()
{
	const float CurrentCoreDrive = GetCurrentCoreDrive();
	if (CurrentCoreDrive <= 0.f)
	{
		return false;
	}

	ApplyCoreDriveChange(-CurrentCoreDrive);
	bWasFullLastFrame = false;
	return true;
}

bool UKRCoreDriveComponent::HasEnoughCoreDrive(float Amount) const
{
	return GetCurrentCoreDrive() >= Amount;
}

// ─────────────────────────────────────────────────────
// 조회 API
// ─────────────────────────────────────────────────────

float UKRCoreDriveComponent::GetCurrentCoreDrive() const
{
	const UKRPlayerAttributeSet* AttrSet = GetPlayerAttributeSet();
	return AttrSet ? AttrSet->GetCoreDrive() : 0.f;
}

float UKRCoreDriveComponent::GetMaxCoreDrive() const
{
	const UKRPlayerAttributeSet* AttrSet = GetPlayerAttributeSet();
	return AttrSet ? AttrSet->GetMaxCoreDrive() : 60.f;
}

float UKRCoreDriveComponent::GetCoreDrivePercent() const
{
	const float MaxCoreDrive = GetMaxCoreDrive();
	return MaxCoreDrive > 0.f ? GetCurrentCoreDrive() / MaxCoreDrive : 0.f;
}

bool UKRCoreDriveComponent::CanUseCoreDriveBurst() const
{
	return GetCoreDrivePercent() >= BurstConfig.BurstRequiredPercent;
}

// ─────────────────────────────────────────────────────
// 내부 함수
// ─────────────────────────────────────────────────────

void UKRCoreDriveComponent::ApplyCoreDriveChange(float Delta)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	const float CurrentCoreDrive = GetCurrentCoreDrive();
	const float MaxCoreDrive = GetMaxCoreDrive();
	const float NewCoreDrive = FMath::Clamp(CurrentCoreDrive + Delta, 0.f, MaxCoreDrive);

	// 직접 어트리뷰트 수정
	ASC->SetNumericAttributeBase(UKRPlayerAttributeSet::GetCoreDriveAttribute(), NewCoreDrive);

	// 변경 이벤트 브로드캐스트
	if (!FMath::IsNearlyEqual(LastCoreDriveValue, NewCoreDrive))
	{
		LastCoreDriveValue = NewCoreDrive;
		OnCoreDriveChanged.Broadcast(NewCoreDrive, MaxCoreDrive);
	}
}

UAbilitySystemComponent* UKRCoreDriveComponent::GetASC() const
{
	if (!CachedASC.IsValid())
	{
		CachedASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	}
	return CachedASC.Get();
}

const UKRPlayerAttributeSet* UKRCoreDriveComponent::GetPlayerAttributeSet() const
{
	if (!CachedAttributeSet.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetASC())
		{
			CachedAttributeSet = ASC->GetSet<UKRPlayerAttributeSet>();
		}
	}
	return CachedAttributeSet.Get();
}

// ─────────────────────────────────────────────────────
// 이벤트 핸들러
// ─────────────────────────────────────────────────────

void UKRCoreDriveComponent::OnDamageDealtEvent(const FGameplayEventData* EventData)
{
	if (!EventData) return;

	if (EventData->EventTag == KRTAG_EVENT_COREDRIVE_CHARGE_ONHIT)
	{
		// EventMagnitude에 데미지가 담겨있음
		ChargeOnHit(EventData->EventMagnitude);
	}
	else if (EventData->EventTag == KRTAG_EVENT_COREDRIVE_CHARGE_ONPARRY)
	{
		ChargeOnPerfectGuard();
	}
	else if (EventData->EventTag == KRTAG_EVENT_COREDRIVE_CHARGE_ONKILL)
	{
		ChargeOnKill();
	}
}

void UKRCoreDriveComponent::OnPerfectGuardEvent(const FGameplayEventData* EventData)
{
	ChargeOnPerfectGuard();
}

void UKRCoreDriveComponent::OnEnemyKilledEvent(const FGameplayEventData* EventData)
{
	ChargeOnKill();
}
