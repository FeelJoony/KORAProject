#include "GAS/Abilities/EnemyAbility/KRGA_EnemySlash.h"
#include "Enemy/KRHumanEnemyCharacter.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UKRGA_EnemySlash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
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

	AKRHumanEnemyCharacter* Enemy = Cast<AKRHumanEnemyCharacter>(ActorInfo->AvatarActor);
	if (!IsValid(Enemy))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (SlashMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("GA Montage"));
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				SlashMontage,
				1.f,
				NAME_None
			);

		ActivateSlash();
	}
}

void UKRGA_EnemySlash::EndAbility(const FGameplayAbilitySpecHandle Handle, 
									const FGameplayAbilityActorInfo* ActorInfo, 
									const FGameplayAbilityActivationInfo ActivationInfo, 
									bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_EnemySlash::ActivateSlash()
{
	MontageTask->OnCompleted.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void UKRGA_EnemySlash::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}