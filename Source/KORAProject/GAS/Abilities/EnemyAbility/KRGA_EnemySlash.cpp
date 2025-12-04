#include "GAS/Abilities/EnemyAbility/KRGA_EnemySlash.h"
#include "Enemy/KRHumanEnemyCharacter.h"
#include "GameplayTag/KREnemyTag.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UKRGA_EnemySlash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
										const FGameplayAbilityActorInfo* ActorInfo, 
										const FGameplayAbilityActivationInfo ActivationInfo, 
										const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("!Commit"));
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
	if (!IsValid(Enemy))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (SlashMontage)
	{
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

	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(Enemy)) return;

	UKRAbilitySystemComponent* EnemyASC = Enemy->GetEnemyAbilitySystemCompoent();
	if (EnemyASC && EnemyASC->HasMatchingGameplayTag(KRTAG_ENEMY_ACTION_SLASH))
	{
		EnemyASC->RemoveLooseGameplayTag(KRTAG_ENEMY_ACTION_SLASH);
	}
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