#include "GAS/Abilities/HeroAbilities/KRGA_HeroGuard.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/KRStaminaComponent.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KREventTag.h"

UKRGA_HeroGuard::UKRGA_HeroGuard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGA_HeroGuard::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 가드 상태 시작
	StartGuardState();

	// 가드 브레이크 이벤트 대기
	GuardBrokenEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, KRTAG_EVENT_COMBAT_GUARDBROKEN, nullptr, false, true);
	if (GuardBrokenEventTask)
	{
		GuardBrokenEventTask->EventReceived.AddDynamic(this, &ThisClass::OnGuardBrokenEventReceived);
		GuardBrokenEventTask->ReadyForActivation();
	}

	// 패리 성공 이벤트 대기
	ParrySuccessEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, KRTAG_EVENT_COMBAT_PARRYSUCCESS, nullptr, false, true);
	if (ParrySuccessEventTask)
	{
		ParrySuccessEventTask->EventReceived.AddDynamic(this, &ThisClass::OnParrySuccessEventReceived);
		ParrySuccessEventTask->ReadyForActivation();
	}

	// 가드 히트 이벤트 대기
	GuardHitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, KRTAG_EVENT_COMBAT_HIT, nullptr, false, true);
	if (GuardHitEventTask)
	{
		GuardHitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnGuardHitEventReceived);
		GuardHitEventTask->ReadyForActivation();
	}

	// 가드 시작 몽타주 재생
	if (GuardStartMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("GuardStart"), GuardStartMontage, 1.0f);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			MontageTask->ReadyForActivation();
		}
	}
}

void UKRGA_HeroGuard::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PerfectGuardWindowTimer);
		World->GetTimerManager().ClearTimer(GuardBreakTimer);
	}

	// 가드 상태 종료
	StopGuardState();

	// 스태미나 회복 상태 복원
	if (UKRStaminaComponent* StaminaComp = GetStaminaComponent())
	{
		StaminaComp->SetRecoveryState(EStaminaRecoveryState::Normal);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────
// 가드 상태 관리
// ─────────────────────────────────────────────────────

void UKRGA_HeroGuard::StartGuardState()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// GUARD 태그 추가
	ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_GUARD);

	// PARRY 태그 추가 (퍼펙트 가드 윈도우)
	ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);

	// 퍼펙트 가드 상태로 설정
	CurrentGuardState = EGuardState::ParryWindow;

	// 퍼펙트 가드 윈도우 타이머 시작
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PerfectGuardWindowTimer,
			this,
			&ThisClass::OnPerfectGuardWindowEnded,
			GuardConfig.PerfectGuardWindow,
			false
		);
	}

	// 가드 중 스태미나 회복 감소
	if (UKRStaminaComponent* StaminaComp = GetStaminaComponent())
	{
		StaminaComp->SetRecoveryState(EStaminaRecoveryState::Guarding);
	}
}

void UKRGA_HeroGuard::StopGuardState()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// 모든 가드 관련 태그 제거
	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GUARD);
	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GUARDBROKEN);

	CurrentGuardState = EGuardState::None;
}

void UKRGA_HeroGuard::OnPerfectGuardWindowEnded()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// PARRY 태그 제거 (더 이상 퍼펙트 가드 불가)
	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);

	// 일반 가드 상태로 전환
	CurrentGuardState = EGuardState::StandardGuard;

	// 가드 루프 몽타주 재생
	if (GuardLoopMontage)
	{
		if (MontageTask)
		{
			MontageTask->EndTask();
		}
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("GuardLoop"), GuardLoopMontage, 1.0f, NAME_None, true);
		if (MontageTask)
		{
			MontageTask->ReadyForActivation();
		}
	}
}

// ─────────────────────────────────────────────────────
// 가드 히트 처리
// ─────────────────────────────────────────────────────

void UKRGA_HeroGuard::OnGuardHit(float IncomingDamage, AActor* Attacker)
{
	if (CurrentGuardState == EGuardState::ParryWindow)
	{
		OnPerfectGuardSuccess(Attacker);
	}
	else if (CurrentGuardState == EGuardState::StandardGuard)
	{
		OnStandardGuardHit(IncomingDamage, Attacker);
	}
}

void UKRGA_HeroGuard::OnPerfectGuardSuccess(AActor* Attacker)
{
	// 퍼펙트 가드 성공 - 스태미나 소모 없음
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// 퍼펙트 가드 넉백 적용 (최소 넉백)
	if (AvatarActor && Attacker && GuardConfig.PerfectGuardKnockback > 0.f)
	{
		UKRMotionWarpingLibrary::SetKnockbackWarpTarget(
			AvatarActor,
			FName("GuardKnockback"),
			Attacker,
			GuardConfig.PerfectGuardKnockback,
			true  // 공격자 바라보기
		);
	}

	// 퍼펙트 가드 몽타주 재생
	if (PerfectGuardMontage)
	{
		if (MontageTask)
		{
			MontageTask->EndTask();
		}
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("PerfectGuard"), PerfectGuardMontage, 1.0f);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			MontageTask->ReadyForActivation();
		}
	}

	// 퍼펙트 가드 Cue 실행
	if (PerfectGuardCueTag.IsValid())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
			ASC->ExecuteGameplayCue(PerfectGuardCueTag, CueParams);
		}
	}

	// 코어드라이브 충전 이벤트 전송
	if (AvatarActor)
	{
		FGameplayEventData ChargeEvent;
		ChargeEvent.EventTag = KRTAG_EVENT_COREDRIVE_CHARGE_ONPARRY;
		ChargeEvent.Instigator = AvatarActor;
		ChargeEvent.Target = Attacker;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			AvatarActor,
			KRTAG_EVENT_COREDRIVE_CHARGE_ONPARRY,
			ChargeEvent
		);
	}

	// 퍼펙트 가드 후에도 윈도우 리셋
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PerfectGuardWindowTimer);
		World->GetTimerManager().SetTimer(
			PerfectGuardWindowTimer,
			this,
			&ThisClass::OnPerfectGuardWindowEnded,
			GuardConfig.PerfectGuardWindow,
			false
		);
	}
}

void UKRGA_HeroGuard::OnStandardGuardHit(float IncomingDamage, AActor* Attacker)
{
	// 스태미나 소모 계산
	float StaminaCost = IncomingDamage * GuardConfig.GuardStaminaCostMultiplier;

	UKRStaminaComponent* StaminaComp = GetStaminaComponent();
	if (StaminaComp)
	{
		// 스태미나가 부족하면 가드 브레이크
		if (!StaminaComp->HasEnoughStamina(StaminaCost))
		{
			TriggerGuardBreak();
			return;
		}

		// 스태미나 소모
		StaminaComp->ConsumeStamina(StaminaCost);
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// 일반 가드 넉백 적용 (표준 넉백)
	if (AvatarActor && Attacker && GuardConfig.StandardGuardKnockback > 0.f)
	{
		UKRMotionWarpingLibrary::SetKnockbackWarpTarget(
			AvatarActor,
			FName("GuardKnockback"),
			Attacker,
			GuardConfig.StandardGuardKnockback,
			true  // 공격자 바라보기
		);
	}

	// 가드 히트 몽타주 재생
	if (GuardHitMontage)
	{
		if (MontageTask)
		{
			MontageTask->EndTask();
		}
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("GuardHit"), GuardHitMontage, 1.0f);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			MontageTask->ReadyForActivation();
		}
	}

	// 가드 히트 Cue 실행
	if (GuardHitCueTag.IsValid())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
			ASC->ExecuteGameplayCue(GuardHitCueTag, CueParams);
		}
	}
}

// ─────────────────────────────────────────────────────
// 가드 브레이크
// ─────────────────────────────────────────────────────

void UKRGA_HeroGuard::TriggerGuardBreak()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// 가드 태그 제거, 가드 브레이크 태그 추가
	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GUARD);
	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
	ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_GUARDBROKEN);

	CurrentGuardState = EGuardState::GuardBroken;

	// 가드 브레이크 몽타주 재생
	if (GuardBreakMontage)
	{
		if (MontageTask)
		{
			MontageTask->EndTask();
		}
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("GuardBreak"), GuardBreakMontage, 1.0f);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			MontageTask->ReadyForActivation();
		}
	}

	// 가드 브레이크 Cue 실행
	if (GuardBreakCueTag.IsValid())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
		ASC->ExecuteGameplayCue(GuardBreakCueTag, CueParams);
	}

	// 가드 브레이크 지속 시간 후 능력 종료
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			GuardBreakTimer,
			this,
			&ThisClass::OnGuardBreakEnded,
			GuardConfig.GuardBreakDuration,
			false
		);
	}
}

void UKRGA_HeroGuard::OnGuardBreakEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// ─────────────────────────────────────────────────────
// 이벤트 핸들러
// ─────────────────────────────────────────────────────

void UKRGA_HeroGuard::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	// 가드 브레이크 중에는 입력 무시
	if (CurrentGuardState == EGuardState::GuardBroken)
	{
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKRGA_HeroGuard::OnGuardBrokenEventReceived(FGameplayEventData Payload)
{
	TriggerGuardBreak();
}

void UKRGA_HeroGuard::OnParrySuccessEventReceived(FGameplayEventData Payload)
{
	AActor* Attacker = const_cast<AActor*>(Payload.Instigator.Get());
	OnPerfectGuardSuccess(Attacker);
}

void UKRGA_HeroGuard::OnGuardHitEventReceived(FGameplayEventData Payload)
{
	// 가드 상태가 아니면 무시
	if (CurrentGuardState == EGuardState::None || CurrentGuardState == EGuardState::GuardBroken)
	{
		return;
	}

	// EventMagnitude에 데미지가 담겨있다고 가정
	float IncomingDamage = Payload.EventMagnitude;
	AActor* Attacker = const_cast<AActor*>(Payload.Instigator.Get());

	OnGuardHit(IncomingDamage, Attacker);
}

void UKRGA_HeroGuard::OnMontageCompleted()
{
	// 가드 브레이크 상태가 아니면 가드 루프로 돌아감
	if (CurrentGuardState != EGuardState::GuardBroken && GuardLoopMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("GuardLoop"), GuardLoopMontage, 1.0f, NAME_None, true);
		if (MontageTask)
		{
			MontageTask->ReadyForActivation();
		}
	}
}

void UKRGA_HeroGuard::OnMontageInterrupted()
{
	// 몽타주가 인터럽트되면 능력 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

// ─────────────────────────────────────────────────────
// 유틸리티
// ─────────────────────────────────────────────────────

UKRStaminaComponent* UKRGA_HeroGuard::GetStaminaComponent() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	return AvatarActor ? AvatarActor->FindComponentByClass<UKRStaminaComponent>() : nullptr;
}
