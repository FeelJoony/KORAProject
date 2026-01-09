#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Hit.h"
#include "Characters/KREnemyCharacter.h"
#include "GameplayTag/KREnemyTag.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UKRGA_Enemy_Hit::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
										const FGameplayAbilityActorInfo* ActorInfo,
										const FGameplayAbilityActivationInfo ActivationInfo,
										const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// AActor* Victim = GetAvatarActorFromActorInfo();
	// if (!TriggerEventData)
	// {
	// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	// 	return;
	// }
	// AActor* Attacker = const_cast<AActor*>(TriggerEventData->Instigator.Get());
	//
	// CachedCueParams = FKRHitReactionCueParams();
	// CachedCueParams.HitDirection = UKRHitReactionLibrary::CalculateHitDirection(Victim, Attacker);
	//
	// // MeleeAttack에서 EventMagnitude로 패킹된 데이터 언패킹
	// // EventMagnitude = HitIntensity(정수부) + KnockbackDistance * 0.001(소수부)
	// // 예: 2.150 → HitIntensity=2, KnockbackDistance=150
	// if (TriggerEventData->EventMagnitude > 0.0f)
	// {
	// 	int32 UnpackedIntensity = FMath::FloorToInt(TriggerEventData->EventMagnitude);
	// 	CachedCueParams.HitIntensity = static_cast<EKRHitIntensity>(FMath::Clamp(
	// 		UnpackedIntensity,
	// 		0,
	// 		static_cast<int32>(EKRHitIntensity::Heavy)
	// 	));
	// }
	// else
	// {
	// 	CachedCueParams.HitIntensity = DefaultHitIntensity;
	// }
	//
	// CachedCueParams.SoundTag = HitSoundTag;
	// CachedCueParams.EffectTag = HitEffectTag;
	// CachedCueParams.EffectSocketName = NAME_None;
	// if (const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult())
	// {
	// 	if (HitResult->BoneName != NAME_None)
	// 	{
	// 		CachedCueParams.EffectSocketName = HitResult->BoneName;
	// 	}
	// }
	//
	// CachedCueParams.HitLocation = Victim ? Victim->GetActorLocation() : FVector::ZeroVector;
	// if (const FHitResult* HitResult = TriggerEventData->ContextHandle.GetHitResult())
	// {
	// 	CachedCueParams.HitLocation = FVector(HitResult->ImpactPoint);
	// }
	//
	// ExecuteHitReactionCue(CachedCueParams);
	
	if (HitMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			HitMontage,
			1.f,
			NAME_None
		);

		ActivationHit();
	}
}

void UKRGA_Enemy_Hit::EndAbility(const FGameplayAbilitySpecHandle Handle,
									const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayAbilityActivationInfo ActivationInfo,
									bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* EnemyASC = GetAbilitySystemComponentFromActorInfo();
	if (EnemyASC && EnemyASC->HasMatchingGameplayTag(KRTAG_ENEMY_AISTATE_HITREACTION))
	{
		EnemyASC->RemoveLooseGameplayTag(KRTAG_ENEMY_AISTATE_HITREACTION);
	}
}

void UKRGA_Enemy_Hit::ActivationHit()
{
	MontageTask->OnCompleted.AddDynamic(this, &UKRGA_Enemy_Hit::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UKRGA_Enemy_Hit::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Enemy_Hit::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Enemy_Hit::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void UKRGA_Enemy_Hit::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// void UKRGA_Enemy_Hit::ExecuteHitReactionCue(const FKRHitReactionCueParams& CueParams)
// {
// 	if (!HitReactionCueTag.IsValid())
// 	{
// 		return;
// 	}
//
// 	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
// 	if (!ASC)
// 	{
// 		return;
// 	}
// 	
// 	FGameplayCueParameters GCParams;
// 	GCParams.Location = CueParams.HitLocation;
// 	GCParams.Normal = CueParams.HitNormal;
// 	GCParams.RawMagnitude = static_cast<float>(CueParams.HitIntensity);
// 	
// 	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
// 	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
// 	
// 	GCParams.AggregatedSourceTags.AddTag(CueParams.SoundTag);
// 	GCParams.AggregatedSourceTags.AddTag(CueParams.EffectTag);
// 	
// 	FString DirectionString = UKRHitReactionLibrary::HitDirectionToString(CueParams.HitDirection);
// 	FGameplayTag DirectionTag = FGameplayTag::RequestGameplayTag(FName(*FString::Printf(TEXT("HitReaction.Direction.%s"), *DirectionString)), false);
// 	if (DirectionTag.IsValid())
// 	{
// 		GCParams.AggregatedSourceTags.AddTag(DirectionTag);
// 	}
//
// 	GCParams.EffectContext = ContextHandle;
// 	ASC->ExecuteGameplayCue(HitReactionCueTag, GCParams);
// }
