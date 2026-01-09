#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Stun.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UKRGA_Enemy_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
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

	if (StunMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			StunMontage,
			1.f,
			NAME_None
		);

		ActivationStun();
	}
}

void UKRGA_Enemy_Stun::EndAbility(const FGameplayAbilitySpecHandle Handle, 
									const FGameplayAbilityActorInfo* ActorInfo, 
									const FGameplayAbilityActivationInfo ActivationInfo, 
									bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Enemy_Stun::ActivationStun()
{
	MontageTask->OnCompleted.AddDynamic(this, &UKRGA_Enemy_Stun::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UKRGA_Enemy_Stun::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Enemy_Stun::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Enemy_Stun::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void UKRGA_Enemy_Stun::OnMontageEnded()
{
	bMontageFinished = true;
	if (bPendingEnd)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UKRGA_Enemy_Stun::ExternalAbilityEnded()
{
	if (bMontageFinished)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		MontageStop(0.25);
		bPendingEnd = true;
	}
}
