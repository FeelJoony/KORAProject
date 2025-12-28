#include "GAS/Abilities/HeroAbilities/KRGA_HeroSprint.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystemComponent.h"
#include "Components/KRStaminaComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRGA_HeroSprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	bSprintStarted = false;
	CurrentChargeTime=0.f;
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (UCharacterMovementComponent* MovementComponent = Char->GetCharacterMovement())
	{
		InitialWalkSpeed = MovementComponent->MaxWalkSpeed;
	}

	// 스태미나 지속 소모 시작
	if (UKRStaminaComponent* StaminaComp = Char->FindComponentByClass<UKRStaminaComponent>())
	{
		StaminaComp->StartContinuousConsumption(SprintStaminaCostPerSecond);
		StaminaComp->SetRecoveryState(EStaminaRecoveryState::Sprinting);
	}

	GetWorld()->GetTimerManager().SetTimer(
		SprintChargeTimerHandle,
		this,
		&UKRGA_HeroSprint::UpdateSprintSpeed,
		0.01f,
		true
	);
}

void UKRGA_HeroSprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
		{
			Move->MaxWalkSpeed = WalkSpeed;
		}

		// 스태미나 지속 소모 중지 및 회복 상태 복원
		if (UKRStaminaComponent* StaminaComp = Char->FindComponentByClass<UKRStaminaComponent>())
		{
			StaminaComp->StopContinuousConsumption();
			StaminaComp->SetRecoveryState(EStaminaRecoveryState::Normal);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(SprintChargeTimerHandle);
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

void UKRGA_HeroSprint::UpdateSprintSpeed()
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char || !Char->GetCharacterMovement())
	{
		GetWorld()->GetTimerManager().ClearTimer(SprintChargeTimerHandle);
		return;
	}

	// 스태미나가 0이면 달리기 중단
	if (UKRStaminaComponent* StaminaComp = Char->FindComponentByClass<UKRStaminaComponent>())
	{
		if (StaminaComp->GetCurrentStamina() <= 0.f)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}
	}

	CurrentChargeTime += 0.01f;
	float Alpha = FMath::Clamp(CurrentChargeTime / SprintChargeTime, 0.0f, 1.0f);
	float NewSpeed = FMath::Lerp(InitialWalkSpeed, SprintSpeed, Alpha);
	Char->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	if (Alpha >= 1.0f)
	{
  		bSprintStarted = true;
		GetWorld()->GetTimerManager().ClearTimer(SprintChargeTimerHandle);
	}
}

void UKRGA_HeroSprint::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
