#include "GAS/Abilities/HeroAbilities/KRGA_HeroSprint.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UKRGA_HeroSprint::UKRGA_HeroSprint()
{
	bSprintStarted=false;
	SprintChargeTime = 0.25f;
	SprintSpeed = 800.f;
	WalkSpeed = 400.f;
}

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
	StartSprintTask = UAbilityTask_WaitDelay::WaitDelay(this, SprintChargeTime);
	StartSprintTask->OnFinish.AddDynamic(this, &UKRGA_HeroSprint::OnSprint);
	StartSprintTask->ReadyForActivation();
}

void UKRGA_HeroSprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
			Move->MaxWalkSpeed = WalkSpeed;
	}

	if (StartSprintTask) StartSprintTask->EndTask();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroSprint::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	if (bSprintStarted)
	{
		for (FGameplayAbilitySpec& JumpSpec : ASC->GetActivatableAbilities())
		{
			const FGameplayTagContainer& JumpGATags = JumpSpec.Ability->GetAssetTags();
			//UE_LOG(LogTemp, Warning, TEXT("Step GA Tags: %s"), *StepGATags.ToString());
			if (JumpGATags.HasTagExact(JumpAbilityTag))
			{
				ASC->TryActivateAbility(JumpSpec.Handle);
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			}
		}
	}
	
	if (IsValid(EndDelayTask))
	{
		EndDelayTask->EndTask();
		EndDelayTask = nullptr;
	}
}

void UKRGA_HeroSprint::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	if (!bSprintStarted)
	{
		for (FGameplayAbilitySpec& StepSpec : ASC->GetActivatableAbilities())
		{
			const FGameplayTagContainer& StepGATags = StepSpec.Ability->GetAssetTags();
			if (StepGATags.HasTagExact(StepAbilityTag))
			{
				ASC->TryActivateAbility(StepSpec.Handle);
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			}
		}
	}

	EndDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 0.2f);
	if (IsValid(EndDelayTask))
	{
		EndDelayTask->OnFinish.AddDynamic(this, &UKRGA_HeroSprint::OnAbilityEnd);
		EndDelayTask->ReadyForActivation();
	}
}

void UKRGA_HeroSprint::OnSprint()
{
	bSprintStarted = true;

	if (ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
			Move->MaxWalkSpeed = SprintSpeed;
	}
}

void UKRGA_HeroSprint::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
