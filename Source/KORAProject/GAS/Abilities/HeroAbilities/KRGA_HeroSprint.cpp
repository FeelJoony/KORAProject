#include "GAS/Abilities/HeroAbilities/KRGA_HeroSprint.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRGA_HeroSprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	ASC->AddLooseGameplayTag(CanStepTag);
	
	UAbilityTask_WaitDelay* DelayStep = UAbilityTask_WaitDelay::WaitDelay(this, 0.15f);
	if (DelayStep)
	{
		DelayStep->OnFinish.AddDynamic(this, &UKRGA_HeroSprint::OnBlockStep);
		DelayStep->ReadyForActivation();
	}
	UAbilityTask_WaitDelay* DelayJump = UAbilityTask_WaitDelay::WaitDelay(this, 0.25f);
	if (DelayJump)
	{
		DelayJump->OnFinish.AddDynamic(this, &UKRGA_HeroSprint::OnAllowJumpStep);
		DelayJump->ReadyForActivation();
	}
}

void UKRGA_HeroSprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(CanStepTag);
		ASC->RemoveLooseGameplayTag(CanJumpTag);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroSprint::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (EndDelayTask)
	{
		EndDelayTask->EndTask();
		EndDelayTask = nullptr;
	}
	
	//Speed Up
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = 600.f; // 기획서 탐색 후 수정요망
		}
	}
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ASC->HasMatchingGameplayTag(CanJumpTag))
		{
			for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
			{
				FGameplayTagContainer SourceTags = Spec.GetDynamicSpecSourceTags();
				if (SourceTags.HasTagExact(CanJumpTag))
				{
					ASC->TryActivateAbility(Spec.Handle);
					break;
				}
			}
		}
		if (ASC->HasMatchingGameplayTag(CanStepTag))
		{
			for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
			{
				FGameplayTagContainer SourceTags = Spec.GetDynamicSpecSourceTags();
				if (SourceTags.HasTagExact(CanStepTag))
				{
					ASC->TryActivateAbility(Spec.Handle);
					break;
				}
			}
		}
	}
}

void UKRGA_HeroSprint::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (EndDelayTask)
	{
		EndDelayTask->EndTask();
		EndDelayTask = nullptr;
	}
	EndDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 1.0f);
	EndDelayTask->OnFinish.AddDynamic(this, &UKRGA_HeroSprint::OnAbilityEnd);
	EndDelayTask->ReadyForActivation();
	
	//Speed Down
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = 400.f;
		}
	}
}

void UKRGA_HeroSprint::OnBlockStep()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(CanStepTag);
	}
}

void UKRGA_HeroSprint::OnAllowJumpStep()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(CanStepTag);
		ASC->AddLooseGameplayTag(CanJumpTag);
	}
}

void UKRGA_HeroSprint::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
