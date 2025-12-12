#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Alert.h"
#include "Characters/KREnemyCharacter.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GameplayTag/KREnemyTag.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UKRGA_Enemy_Alert::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	if (!IsValid(Enemy))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	ActivationAlert();
}

void UKRGA_Enemy_Alert::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Enemy_Alert::ActivationAlert()
{
	if (AlertMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AlertMontage,
			1.f,
			NAME_None
		);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &UKRGA_Enemy_Alert::OnMontageEnded);
			MontageTask->OnCancelled.AddDynamic(this, &UKRGA_Enemy_Alert::OnMontageEnded);
			MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Enemy_Alert::OnMontageEnded);
			MontageTask->ReadyForActivation();
		}
	}
}

void UKRGA_Enemy_Alert::OnMontageEnded()
{
	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Enemy->GetEnemyAbilitySystemCompoent();
	if (!IsValid(EnemyASC))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	if(EnemyASC->HasMatchingGameplayTag(AlertAbilityTag))
	{
		ActivationAlert();
	}
}

void UKRGA_Enemy_Alert::ExternalAbilityEnded()
{
	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Enemy->GetEnemyAbilitySystemCompoent();
	if (Enemy && AlertMontage)
	{
		if (UAnimInstance* AnimInst = Enemy->GetMesh()->GetAnimInstance())
		{
			AnimInst->Montage_Stop(0.1f, AlertMontage);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}