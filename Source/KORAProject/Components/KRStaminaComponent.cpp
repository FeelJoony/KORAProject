#include "Components/KRStaminaComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GameplayTag/KREventTag.h"
#include "AbilitySystemBlueprintLibrary.h"

UKRStaminaComponent::UKRStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UKRStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기 스태미나 값 캐싱
	LastStaminaValue = GetCurrentStamina();
}

void UKRStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 지속 소모 처리 (달리기)
	if (bIsContinuousConsuming)
	{
		TickContinuousConsumption(DeltaTime);
	}

	// 회복 처리
	TickRecovery(DeltaTime);
}

// ─────────────────────────────────────────────────────
// 소모 API
// ─────────────────────────────────────────────────────

bool UKRStaminaComponent::ConsumeStamina(float Amount)
{
	if (Amount <= 0.f) return true;

	const float CurrentStamina = GetCurrentStamina();
	if (CurrentStamina < Amount)
	{
		return false;
	}

	ApplyStaminaChange(-Amount);

	// 동작 후 회복 딜레이 시작
	StartRecoveryDelay(RecoveryConfig.ActionRecoveryDelay);

	// 스태미나 고갈 체크
	if (GetCurrentStamina() <= 0.f)
	{
		OnStaminaDepleted.Broadcast();

		// 스태미나 고갈 이벤트 전송
		if (AActor* Owner = GetOwner())
		{
			FGameplayEventData EventData;
			EventData.EventTag = KRTAG_EVENT_STAMINA_EMPTY;
			EventData.Instigator = Owner;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, KRTAG_EVENT_STAMINA_EMPTY, EventData);
		}
	}

	return true;
}

bool UKRStaminaComponent::HasEnoughStamina(float Amount) const
{
	return GetCurrentStamina() >= Amount;
}

void UKRStaminaComponent::StartContinuousConsumption(float RatePerSecond)
{
	bIsContinuousConsuming = true;
	ContinuousConsumeRate = RatePerSecond;
}

void UKRStaminaComponent::StopContinuousConsumption()
{
	bIsContinuousConsuming = false;
	ContinuousConsumeRate = 0.0f;
}

// ─────────────────────────────────────────────────────
// 회복 API
// ─────────────────────────────────────────────────────

void UKRStaminaComponent::SetRecoveryState(EStaminaRecoveryState NewState)
{
	if (CurrentRecoveryState == NewState) return;

	CurrentRecoveryState = NewState;

	// 상태에 따른 회복 딜레이 시작
	switch (NewState)
	{
	case EStaminaRecoveryState::Guarding:
		StartRecoveryDelay(RecoveryConfig.GuardRecoveryDelay);
		break;
	case EStaminaRecoveryState::Normal:
		StartRecoveryDelay(RecoveryConfig.DefaultRecoveryDelay);
		break;
	default:
		break;
	}
}

void UKRStaminaComponent::StartRecoveryDelay(float DelaySeconds)
{
	if (DelaySeconds < 0.f)
	{
		DelaySeconds = RecoveryConfig.DefaultRecoveryDelay;
	}

	bIsRecoveryDelayed = true;

	// 기존 타이머 취소
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RecoveryDelayTimerHandle);
		World->GetTimerManager().SetTimer(
			RecoveryDelayTimerHandle,
			this,
			&UKRStaminaComponent::OnRecoveryDelayEnded,
			DelaySeconds,
			false
		);
	}
}

void UKRStaminaComponent::OnRecoveryDelayEnded()
{
	bIsRecoveryDelayed = false;
}

// ─────────────────────────────────────────────────────
// 조회 API
// ─────────────────────────────────────────────────────

float UKRStaminaComponent::GetCurrentStamina() const
{
	const UKRPlayerAttributeSet* AttrSet = GetPlayerAttributeSet();
	return AttrSet ? AttrSet->GetCurrentStamina() : 0.f;
}

float UKRStaminaComponent::GetMaxStamina() const
{
	const UKRPlayerAttributeSet* AttrSet = GetPlayerAttributeSet();
	return AttrSet ? AttrSet->GetMaxStamina() : 100.f;
}

float UKRStaminaComponent::GetStaminaPercent() const
{
	const float MaxStamina = GetMaxStamina();
	return MaxStamina > 0.f ? GetCurrentStamina() / MaxStamina : 0.f;
}

// ─────────────────────────────────────────────────────
// 내부 함수
// ─────────────────────────────────────────────────────

float UKRStaminaComponent::GetCurrentRecoveryRate() const
{
	switch (CurrentRecoveryState)
	{
	case EStaminaRecoveryState::Normal:
		return RecoveryConfig.BaseRecoveryRate;

	case EStaminaRecoveryState::Guarding:
		return RecoveryConfig.BaseRecoveryRate * RecoveryConfig.GuardingRecoveryMultiplier;

	case EStaminaRecoveryState::Sprinting:
	case EStaminaRecoveryState::ActionCooldown:
	default:
		return 0.f;
	}
}

void UKRStaminaComponent::ApplyStaminaChange(float Delta)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;

	const float CurrentStamina = GetCurrentStamina();
	const float MaxStamina = GetMaxStamina();
	const float NewStamina = FMath::Clamp(CurrentStamina + Delta, 0.f, MaxStamina);

	// 직접 어트리뷰트 수정
	ASC->SetNumericAttributeBase(UKRPlayerAttributeSet::GetCurrentStaminaAttribute(), NewStamina);

	// 변경 이벤트 브로드캐스트
	if (!FMath::IsNearlyEqual(LastStaminaValue, NewStamina))
	{
		LastStaminaValue = NewStamina;
		OnStaminaChanged.Broadcast(NewStamina, MaxStamina);
	}
}

void UKRStaminaComponent::TickRecovery(float DeltaTime)
{
	// 회복 딜레이 중이면 스킵
	if (bIsRecoveryDelayed) return;

	// 지속 소모 중이면 회복 없음
	if (bIsContinuousConsuming) return;

	// 현재 회복률 계산
	const float RecoveryRate = GetCurrentRecoveryRate();
	if (RecoveryRate <= 0.f) return;

	// 이미 최대치면 스킵
	const float CurrentStamina = GetCurrentStamina();
	const float MaxStamina = GetMaxStamina();
	if (CurrentStamina >= MaxStamina) return;

	// 회복 적용
	const float RecoveryAmount = RecoveryRate * DeltaTime;
	ApplyStaminaChange(RecoveryAmount);
}

void UKRStaminaComponent::TickContinuousConsumption(float DeltaTime)
{
	if (ContinuousConsumeRate <= 0.f) return;

	const float ConsumeAmount = ContinuousConsumeRate * DeltaTime;
	ApplyStaminaChange(-ConsumeAmount);

	// 스태미나 고갈 체크
	if (GetCurrentStamina() <= 0.f)
	{
		StopContinuousConsumption();
		OnStaminaDepleted.Broadcast();

		// 스태미나 고갈 이벤트 전송
		if (AActor* Owner = GetOwner())
		{
			FGameplayEventData EventData;
			EventData.EventTag = KRTAG_EVENT_STAMINA_EMPTY;
			EventData.Instigator = Owner;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, KRTAG_EVENT_STAMINA_EMPTY, EventData);
		}
	}
}

UAbilitySystemComponent* UKRStaminaComponent::GetASC() const
{
	if (!CachedASC.IsValid())
	{
		CachedASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	}
	return CachedASC.Get();
}

const UKRPlayerAttributeSet* UKRStaminaComponent::GetPlayerAttributeSet() const
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
