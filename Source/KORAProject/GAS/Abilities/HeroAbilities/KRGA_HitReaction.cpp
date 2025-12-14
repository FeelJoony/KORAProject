#include "GAS/Abilities/HeroAbilities/KRGA_HitReaction.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRAbilityTag.h"

UKRGA_HitReaction::UKRGA_HitReaction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(KRTAG_ABILITY_HITREACTION);

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

	EKRHitDirection HitDir = CalculateHitDirection(TriggerEventData);

	UAnimMontage* SelectedMontage = GetMontageByDirection(HitDir);

	if (!SelectedMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
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
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true ,false);
}

void UKRGA_HitReaction::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true ,true);
}

EKRHitDirection UKRGA_HitReaction::CalculateHitDirection(const FGameplayEventData* TriggerEventData)
{
	if (!TriggerEventData)
	{
		return EKRHitDirection::Front;
	}

	AActor* Victim = GetAvatarActorFromActorInfo();
	const AActor* Attacker = TriggerEventData->Instigator.Get();

	if (!Victim || !Attacker)
	{
		return EKRHitDirection::Front;
	}

	const FVector VictimForward = Victim->GetActorForwardVector();
	FVector ToAttacker = Attacker->GetActorLocation() - Victim->GetActorLocation();
	ToAttacker.Z = 0.f;
	ToAttacker.Normalize();

	const float ForwardDot = FVector::DotProduct(VictimForward, ToAttacker);
	const float RightDot = FVector::DotProduct(FVector::CrossProduct(FVector::UpVector, VictimForward), ToAttacker);

	if (ForwardDot >= 0.707f)
	{
		return EKRHitDirection::Front;
	}
	if (ForwardDot >= -0.707f)
	{
		return EKRHitDirection::Back;
	}

	return (RightDot > 0.f) ? EKRHitDirection::Right : EKRHitDirection::Left;
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
