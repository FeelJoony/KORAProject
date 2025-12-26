#include "GAS/Abilities/HeroAbilities/KRGA_Death.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/KRCurrencyComponent.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameFramework/Character.h"

UKRGA_Death::UKRGA_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_DEATH);
	SetAssetTags(Tags);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UKRGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true ,true);
		return;
	}

	if (DeathEffectClass)
	{
		FGameplayEffectSpecHandle DeathSpec = MakeOutgoingGameplayEffectSpec(DeathEffectClass, GetAbilityLevel());
		if (DeathSpec.IsValid())
		{
			(void)ApplyGameplayEffectSpecToOwner(
				Handle,
				ActorInfo,
				ActivationInfo,
				DeathSpec);
		}
	}

	if (UKRCurrencyComponent* CurrencyComp = GetAvatarActorFromActorInfo()->FindComponentByClass<UKRCurrencyComponent>())
	{
		CurrencyComp->HandleDeath();
	}

	if (DeathMontage)
	{
		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			DeathMontage,
			1.0f,
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
	else
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	}
}

void UKRGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bStartRagdollOnEnd)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->GetMesh()->SetSimulatePhysics(true);
			Character->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Death::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_Death::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
