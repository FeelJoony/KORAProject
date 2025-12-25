#include "GAS/Abilities/HeroAbilities/KRGA_HitReaction.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRAbilityTag.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

UKRGA_HitReaction::UKRGA_HitReaction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_HITREACTION);
	SetAssetTags(Tags);
	HitReactionCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Combat.HitReaction")); // UILevel과 병합 후 태그 추가 예정

	ActivationPolicy = EKRAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UKRGA_HitReaction::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!TriggerEventData || !TriggerEventData->EventTag.MatchesTag(KRTAG_EVENT_COMBAT_HITREACTION))
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReaction Ability Activated with Invalid Tag or Data!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* Victim = GetAvatarActorFromActorInfo();
	AActor* Attacker = const_cast<AActor*>(TriggerEventData->Instigator.Get());
	
	CachedCueParams = FKRHitReactionCueParams();
	CachedCueParams.HitDirection = UKRHitReactionLibrary::CalculateHitDirection(Victim, Attacker);

	// MeleeAttack에서 EventMagnitude로 전달된 HitIntensity 사용, 없으면 기본값
	if (TriggerEventData->EventMagnitude > 0.0f)
	{
		CachedCueParams.HitIntensity = static_cast<EKRHitIntensity>(FMath::Clamp(
			static_cast<int32>(TriggerEventData->EventMagnitude),
			0,
			static_cast<int32>(EKRHitIntensity::Heavy)
		));
	}
	else
	{
		CachedCueParams.HitIntensity = DefaultHitIntensity;
	}
	CachedCueParams.SoundTag = HitSoundTag;
	CachedCueParams.EffectTag = HitEffectTag;
	CachedCueParams.EffectSocketName = EffectSocketName;
	CachedCueParams.KnockbackDistance = bUseKnockback ? KnockbackDistance : 0.0f;
	CachedCueParams.HitStopDuration = bUseHitStop ? HitStopDuration : 0.0f;
	
	CachedCueParams.HitLocation = Victim ? Victim->GetActorLocation() : FVector::ZeroVector;
	if (const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult())
	{
		CachedCueParams.HitLocation = FVector(HitResult->ImpactPoint);
		CachedCueParams.HitNormal = FVector(HitResult->ImpactNormal);
	}
	else if (Attacker && Victim)
	{
		CachedCueParams.HitNormal = (Victim->GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal();
	}
	
	UAnimMontage* SelectedMontage = GetMontageByDirection(CachedCueParams.HitDirection);
	if (!SelectedMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	ExecuteHitReactionCue(CachedCueParams);
	
	if (bUseHitStop)
	{
		ApplyHitStop();
	}
	
	if (bUseKnockback)
	{
		ApplyKnockback(CachedCueParams.HitNormal);
	}
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		SelectedMontage,
		1.f,
		NAME_None,
		false
	);

	if (Task)
	{
		Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		Task->ReadyForActivation();
	}
}

void UKRGA_HitReaction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HitReaction::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_HitReaction::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

UAnimMontage* UKRGA_HitReaction::GetMontageByDirection(EKRHitDirection Direction) const
{
	switch (Direction)
	{
	case EKRHitDirection::Front:
		return MontageFront;
	case EKRHitDirection::Back:
		return MontageBack;
	case EKRHitDirection::Left:
		return MontageLeft;
	case EKRHitDirection::Right:
		return MontageRight;
	default:
		return MontageFront;
	}
}

void UKRGA_HitReaction::ExecuteHitReactionCue(const FKRHitReactionCueParams& CueParams)
{
	if (!HitReactionCueTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}
	
	FGameplayCueParameters GCParams;
	GCParams.Location = CueParams.HitLocation;
	GCParams.Normal = CueParams.HitNormal;
	GCParams.RawMagnitude = static_cast<float>(CueParams.HitIntensity);  // 강도를 Magnitude로 전달
	
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	
	GCParams.AggregatedSourceTags.AddTag(CueParams.SoundTag);
	GCParams.AggregatedSourceTags.AddTag(CueParams.EffectTag);
	
	FString DirectionString = UKRHitReactionLibrary::HitDirectionToString(CueParams.HitDirection);
	FGameplayTag DirectionTag = FGameplayTag::RequestGameplayTag(FName(*FString::Printf(TEXT("HitReaction.Direction.%s"), *DirectionString)), false);
	if (DirectionTag.IsValid())
	{
		GCParams.AggregatedSourceTags.AddTag(DirectionTag);
	}

	GCParams.EffectContext = ContextHandle;
	ASC->ExecuteGameplayCue(HitReactionCueTag, GCParams);
}

void UKRGA_HitReaction::ApplyKnockback(const FVector& HitDirection)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}
	
	FVector KnockbackDirection = HitDirection;
	KnockbackDirection.Z = 0.f;
	KnockbackDirection.Normalize();

	FVector KnockbackLocation = AvatarActor->GetActorLocation() + (KnockbackDirection * KnockbackDistance);
	
	if (bUseMotionWarping)
	{
		if (UMotionWarpingComponent* MWComp = GetMotionWarpingComponent())
		{
			FMotionWarpingTarget Target;
			Target.Name = WarpTargetName;
			Target.Location = KnockbackLocation;
			Target.Rotation = AvatarActor->GetActorRotation();

			MWComp->AddOrUpdateWarpTarget(Target);
		}
	}
	else
	{
		if (ACharacter* Character = Cast<ACharacter>(AvatarActor))
		{
			FVector LaunchVelocity = KnockbackDirection * (KnockbackDistance * 5.0f);
			LaunchVelocity.Z = 100.0f;
			Character->LaunchCharacter(LaunchVelocity, true, true);
		}
	}
}

void UKRGA_HitReaction::ApplyHitStop()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}
	
	AvatarActor->CustomTimeDilation = 0.01f;
	
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		[WeakActor = TWeakObjectPtr<AActor>(AvatarActor)]()
		{
			if (AActor* Actor = WeakActor.Get())
			{
				Actor->CustomTimeDilation = 1.0f;
			}
		},
		HitStopDuration,
		false
	);
}

UMotionWarpingComponent* UKRGA_HitReaction::GetMotionWarpingComponent() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return nullptr;
	}

	return AvatarActor->FindComponentByClass<UMotionWarpingComponent>();
}
