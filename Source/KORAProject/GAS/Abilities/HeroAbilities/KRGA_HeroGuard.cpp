#include "GAS/Abilities/HeroAbilities/KRGA_HeroGuard.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/KRStaminaComponent.h"
#include "Characters/KRHeroCharacter.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

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

	// 입력 해제 플래그 초기화
	bInputReleased = false;

	StartGuardState();

	GuardBrokenEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, KRTAG_EVENT_COMBAT_GUARDBROKEN, nullptr, false, true);
	if (GuardBrokenEventTask)
	{
		GuardBrokenEventTask->EventReceived.AddDynamic(this, &ThisClass::OnGuardBrokenEventReceived);
		GuardBrokenEventTask->ReadyForActivation();
	}

	ParrySuccessEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, KRTAG_EVENT_COMBAT_PARRYSUCCESS, nullptr, false, true);
	if (ParrySuccessEventTask)
	{
		ParrySuccessEventTask->EventReceived.AddDynamic(this, &ThisClass::OnParrySuccessEventReceived);
		ParrySuccessEventTask->ReadyForActivation();
	}

	GuardHitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, KRTAG_EVENT_COMBAT_HIT, nullptr, false, true);
	if (GuardHitEventTask)
	{
		GuardHitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnGuardHitEventReceived);
		GuardHitEventTask->ReadyForActivation();
	}

	if (GuardStartMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("GuardStart"), GuardStartMontage, 1.0f);
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
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PerfectGuardWindowTimer);
		World->GetTimerManager().ClearTimer(GuardBreakTimer);
	}

	if (AKRHeroCharacter* Hero = GetKRHeroCharacter())
    {
        UKRMotionWarpingLibrary::RemoveWarpTarget(Hero, FName("GuardKnockback"));
    }

	StopGuardState();

	if (UKRStaminaComponent* StaminaComp = GetStaminaComponent())
	{
		StaminaComp->SetRecoveryState(EStaminaRecoveryState::Normal);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroGuard::StartGuardState()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_GUARD);
		ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
	}

	CurrentGuardState = EGuardState::ParryWindow;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(PerfectGuardWindowTimer, this, &ThisClass::OnPerfectGuardWindowEnded, GuardConfig.PerfectGuardWindow, false);
	}

	if (UKRStaminaComponent* StaminaComp = GetStaminaComponent())
	{
		StaminaComp->SetRecoveryState(EStaminaRecoveryState::Guarding);
	}
}

void UKRGA_HeroGuard::StopGuardState()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GUARD);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GUARDBROKEN);
	}
	CurrentGuardState = EGuardState::None;
}

void UKRGA_HeroGuard::OnPerfectGuardWindowEnded()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
	}

	CurrentGuardState = EGuardState::StandardGuard;

	if (GuardLoopMontage)
	{
		StopCurrentMontageTask();
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("GuardLoop"), GuardLoopMontage, 1.0f, NAME_None, true);
		if (MontageTask)
		{
			MontageTask->ReadyForActivation();
		}
	}
}

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
	// 퍼펙트 가드 윈도우 즉시 종료 (첫 번째 공격만 퍼펙트 가드 가능)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PerfectGuardWindowTimer);
	}

	// Parry 태그 제거 (더 이상 퍼펙트 가드 불가)
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
	}

	// 상태 변경: PerfectGuardHit (HitReaction 재생 중)
	CurrentGuardState = EGuardState::PerfectGuardHit;

	ApplyKnockback(Attacker, GuardConfig.PerfectGuardKnockback);

	if (PerfectGuardMontage)
	{
		StopCurrentMontageTask();
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("PerfectGuard"), PerfectGuardMontage, 1.0f);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnHitReactionCompleted);
			MontageTask->ReadyForActivation();
		}
	}

	if (PerfectGuardCueTag.IsValid() && GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(PerfectGuardCueTag, CueParams);
	}

	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		FGameplayEventData ChargeEvent;
		ChargeEvent.EventTag = KRTAG_EVENT_COREDRIVE_CHARGE_ONPARRY;
		ChargeEvent.Instigator = AvatarActor;
		ChargeEvent.Target = Attacker;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AvatarActor, KRTAG_EVENT_COREDRIVE_CHARGE_ONPARRY, ChargeEvent);
	}

	FKRUIMessage_Guard GuardMessage;
	GuardMessage.TargetActor = GetAvatarActorFromActorInfo();
	GuardMessage.bGuardSuccess = true;
	GuardMessage.bPerfectGuard = true;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::Guard(), GuardMessage);
}

void UKRGA_HeroGuard::OnStandardGuardHit(float IncomingDamage, AActor* Attacker)
{
	if (UKRStaminaComponent* StaminaComp = GetStaminaComponent())
	{
		float StaminaCost = IncomingDamage * GuardConfig.GuardStaminaCostMultiplier;
		if (!StaminaComp->HasEnoughStamina(StaminaCost))
		{
			TriggerGuardBreak();
			return;
		}
		StaminaComp->ConsumeStamina(StaminaCost);
	}

	// 상태 변경: StandardGuardHit (HitReaction 재생 중)
	CurrentGuardState = EGuardState::StandardGuardHit;

	ApplyKnockback(Attacker, GuardConfig.StandardGuardKnockback);

	if (GuardHitMontage)
	{
		StopCurrentMontageTask();
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("GuardHit"), GuardHitMontage, 1.0f);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnHitReactionCompleted);
			MontageTask->ReadyForActivation();
		}
	}

	if (GuardHitCueTag.IsValid() && GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GuardHitCueTag, CueParams);
	}

	FKRUIMessage_Guard GuardMessage;
	GuardMessage.TargetActor = GetAvatarActorFromActorInfo();
	GuardMessage.bGuardSuccess = true;
	GuardMessage.bPerfectGuard = false;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::Guard(), GuardMessage);
}

void UKRGA_HeroGuard::ApplyKnockback(AActor* Attacker, float KnockbackDistance)
{
    AKRHeroCharacter* Hero = GetKRHeroCharacter();
    if (Hero && Attacker && KnockbackDistance > 0.f)
    {
        // 넉백 방향 계산 (공격자로부터 멀어지는 방향)
        const FVector AttackerLocation = Attacker->GetActorLocation();
        const FVector HeroLocation = Hero->GetActorLocation();
        FVector KnockbackDirection = (HeroLocation - AttackerLocation);
        KnockbackDirection.Z = 0; // 수평으로만 밀려나도록 Z값 초기화
        KnockbackDirection.Normalize();

        // 최종 목표 위치 설정
        const FVector WarpTargetLocation = HeroLocation + KnockbackDirection * KnockbackDistance;
        
        // 모션 워핑 타겟 설정 (타겟 이름은 몽타주와 일치해야 함)
        UKRMotionWarpingLibrary::SetWarpTargetLocation(Hero, FName("GuardKnockback"), WarpTargetLocation);
    }
}

void UKRGA_HeroGuard::TriggerGuardBreak()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GUARD);
	ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
	ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_GUARDBROKEN);

	CurrentGuardState = EGuardState::GuardBroken;

	if (GuardBreakMontage)
	{
		StopCurrentMontageTask();
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("GuardBreak"), GuardBreakMontage, 1.0f);
		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			MontageTask->ReadyForActivation();
		}
	}

	if (GuardBreakCueTag.IsValid())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
		ASC->ExecuteGameplayCue(GuardBreakCueTag, CueParams);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(GuardBreakTimer, this, &ThisClass::OnGuardBreakEnded, GuardConfig.GuardBreakDuration, false);
	}
}

void UKRGA_HeroGuard::OnGuardBreakEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_HeroGuard::InputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	// HitReaction 중 버튼을 뗐다가 다시 누른 경우
	if (!bInputReleased)
	{
		return;
	}

	bInputReleased = false;

	if (CurrentGuardState == EGuardState::PerfectGuardHit)
	{
		// 퍼펙트 가드 HitReaction 중 다시 누르면 새 패리 윈도우 시작
		CurrentGuardState = EGuardState::ParryWindow;

		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_PARRY);
		}

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
	}
	// StandardGuardHit 상태에서는 bInputReleased = false만 설정
	// (패리 윈도우 없이 일반 가드 유지, HitReaction 완료 후 RestartGuard 호출됨)
}

void UKRGA_HeroGuard::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	// HitReaction 재생 중이거나 GuardBroken 상태에서는 즉시 종료하지 않음
	if (CurrentGuardState == EGuardState::GuardBroken ||
		CurrentGuardState == EGuardState::PerfectGuardHit ||
		CurrentGuardState == EGuardState::StandardGuardHit)
	{
		bInputReleased = true;  // 플래그만 설정, HitReaction 완료 후 종료
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
	// 가드 불가 상태
	if (CurrentGuardState == EGuardState::None ||
		CurrentGuardState == EGuardState::GuardBroken)
	{
		return;
	}

	// HitReaction 재생 중 추가 공격 처리
	if (CurrentGuardState == EGuardState::PerfectGuardHit ||
		CurrentGuardState == EGuardState::StandardGuardHit)
	{
		// 버튼을 누르고 있으면 일반 가드로 막을 수 있음
		if (!bInputReleased)
		{
			float IncomingDamage = Payload.EventMagnitude;
			AActor* Attacker = const_cast<AActor*>(Payload.Instigator.Get());
			OnStandardGuardHit(IncomingDamage, Attacker);
		}
		// 버튼을 뗀 상태면 무시 (HitReaction 완료 후 GA 종료 예정)
		return;
	}

	float IncomingDamage = Payload.EventMagnitude;
	AActor* Attacker = const_cast<AActor*>(Payload.Instigator.Get());
	OnGuardHit(IncomingDamage, Attacker);
}

void UKRGA_HeroGuard::OnMontageCompleted()
{
	// GuardStart/GuardLoop 몽타주 완료 시 GuardLoop로 전환
	// (HitReaction 완료는 OnHitReactionCompleted에서 처리)
	if (CurrentGuardState == EGuardState::ParryWindow ||
		CurrentGuardState == EGuardState::StandardGuard)
	{
		if (GuardLoopMontage)
		{
			MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, FName("GuardLoop"), GuardLoopMontage, 1.0f, NAME_None, true);
			if (MontageTask)
			{
				MontageTask->ReadyForActivation();
			}
		}
	}
}

void UKRGA_HeroGuard::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

UKRStaminaComponent* UKRGA_HeroGuard::GetStaminaComponent() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	return AvatarActor ? AvatarActor->FindComponentByClass<UKRStaminaComponent>() : nullptr;
}

AKRHeroCharacter* UKRGA_HeroGuard::GetKRHeroCharacter() const
{
	return GetAvatarActorFromActorInfo() ? Cast<AKRHeroCharacter>(GetAvatarActorFromActorInfo()) : nullptr;
}

void UKRGA_HeroGuard::StopCurrentMontageTask()
{
	if (MontageTask)
	{
		MontageTask->OnInterrupted.RemoveDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.RemoveDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCompleted.RemoveDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnCompleted.RemoveDynamic(this, &ThisClass::OnHitReactionCompleted);
		MontageTask->EndTask();
	}
}

void UKRGA_HeroGuard::OnHitReactionCompleted()
{
	if (bInputReleased)
	{
		// 버튼을 뗀 상태면 GA 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		// 버튼 유지 중이면 일반 가드 상태로 복귀
		RestartGuard();
	}
}

void UKRGA_HeroGuard::RestartGuard()
{
	// 일반 가드 상태로 복귀 (퍼펙트 가드 윈도우 없음)
	CurrentGuardState = EGuardState::StandardGuard;

	// Guard 태그는 이미 존재하므로 다시 추가하지 않음 (카운트 증가 방지)
	// Parry 태그도 추가하지 않음 - 일반 가드만 유지

	// GuardLoop 몽타주 재생
	if (GuardLoopMontage)
	{
		StopCurrentMontageTask();
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("GuardLoop"), GuardLoopMontage, 1.0f, NAME_None, true);
		if (MontageTask)
		{
			MontageTask->ReadyForActivation();
		}
	}
}