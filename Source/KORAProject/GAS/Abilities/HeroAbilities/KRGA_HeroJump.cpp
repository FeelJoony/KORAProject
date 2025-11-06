#include "GAS/Abilities/HeroAbilities/KRGA_HeroJump.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h"
#include "GameFramework/Character.h"

void UKRGA_HeroJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
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
	Character->Jump();
	UAbilityTask_WaitMovementModeChange* WaitLandedTask = UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(this, MOVE_Walking); //
	WaitLandedTask->OnChange.AddDynamic(this, &UKRGA_HeroJump::OnLanded);
	WaitLandedTask->ReadyForActivation();
}

void UKRGA_HeroJump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroJump::OnLanded(EMovementMode NewMovementMode)
{
	if (RollMontage)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		if (Character)
		{
			UAbilityTask_PlayMontageAndWait* MontageTask  = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, NAME_None, RollMontage);

			MontageTask->OnCompleted.AddDynamic(this, &UKRGA_HeroJump::OnMontageEnded);
			MontageTask->OnCancelled.AddDynamic(this, &UKRGA_HeroJump::OnMontageEnded);
			MontageTask->OnInterrupted.AddDynamic(this, &UKRGA_HeroJump::OnMontageEnded);
			MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_HeroJump::OnMontageEnded);
			MontageTask->ReadyForActivation();
		}
	}
}
void UKRGA_HeroJump::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
