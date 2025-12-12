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

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	if (Enemy->HitMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			Enemy->HitMontage,
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

	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(Enemy)) return;

	UKRAbilitySystemComponent* EnemyASC = Enemy->GetEnemyAbilitySystemCompoent();
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