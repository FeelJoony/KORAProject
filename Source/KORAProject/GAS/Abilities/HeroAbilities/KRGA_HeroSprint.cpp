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
	UAbilityTask_WaitDelay* DelayAllowStep = UAbilityTask_WaitDelay::WaitDelay(this, 0.01f);
	if (DelayAllowStep)
	{
		DelayAllowStep->OnFinish.AddDynamic(this, &UKRGA_HeroSprint::OnAllowStep);
		DelayAllowStep->ReadyForActivation();
	}
	UAbilityTask_WaitDelay* DelayBlockStep = UAbilityTask_WaitDelay::WaitDelay(this, 0.15f);
	if (DelayBlockStep)
	{
		DelayBlockStep->OnFinish.AddDynamic(this, &UKRGA_HeroSprint::OnBlockStep);
		DelayBlockStep->ReadyForActivation();
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
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (IsValid(EndDelayTask))
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
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	if (ASC->HasMatchingGameplayTag(CooldownJumpTag))
	{
		for (FGameplayAbilitySpec& StepSpec : ASC->GetActivatableAbilities())
		{
			const FGameplayTagContainer& StepGATags = StepSpec.Ability->GetAssetTags();
			//UE_LOG(LogTemp, Warning, TEXT("Step GA Tags: %s"), *StepGATags.ToString());
			if (StepGATags.HasTagExact(StepAbilityTag))
			{
				ASC->TryActivateAbility(StepSpec.Handle);
				return;
			}
		}
	}
	else
	{
		for (FGameplayAbilitySpec& JumpSpec : ASC->GetActivatableAbilities())
		{
			const FGameplayTagContainer& JumpGATags = JumpSpec.Ability->GetAssetTags();
			if (JumpGATags.HasTagExact(JumpAbilityTag))
			{
				ASC->TryActivateAbility(JumpSpec.Handle);
				return;
			}
		}
	}
}

void UKRGA_HeroSprint::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (IsValid(EndDelayTask))
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

void UKRGA_HeroSprint::OnAllowStep()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(CanStepTag);
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
