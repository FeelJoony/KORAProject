// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/HeroAbilities/KRGA_HeroGuard.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRGA_HeroGuard::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CachedPlayerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	if (!CachedPlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (UCharacterMovementComponent* MovementComp = CachedPlayerCharacter->GetCharacterMovement())
	{
		PreviousMaxWalkSpeed = MovementComp->MaxWalkSpeed;
		MovementComp->MaxWalkSpeed = GuardMaxWalkSpeed;
	}

	LockCameraRotation();

	if (GuardStartMontage)
	{
		StartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("GuardStartMontage"), GuardStartMontage
		);

		StartMontageTask->OnCompleted.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);
		StartMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);
		StartMontageTask->OnCancelled.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);
		StartMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);

		StartMontageTask->ReadyForActivation();
	}
	else
	{
		OnGuardMontageLoop();
	}
}

void UKRGA_HeroGuard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UnlockCameraRotation();

	if (IsValid(LoopMontageTask))
	{
		LoopMontageTask->EndTask();
	}
	if (IsValid(HitMontageTask))
	{
		HitMontageTask->EndTask();
	}
	if (CachedPlayerCharacter && CachedPlayerCharacter->GetCharacterMovement())
	{
		CachedPlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PreviousMaxWalkSpeed;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroGuard::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (IsValid(LoopMontageTask))
	{
		LoopMontageTask->EndTask();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}

void UKRGA_HeroGuard::OnGuardMontageLoop()
{
	if (GuardLoopMontage)
	{
		LoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("GuardLoopMontage"), GuardLoopMontage
		);

		LoopMontageTask->OnCompleted.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);
		LoopMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);
		LoopMontageTask->OnCancelled.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);
		LoopMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_HeroGuard::OnGuardMontageLoop);

		LoopMontageTask->ReadyForActivation();
	}
}

void UKRGA_HeroGuard::LockCameraRotation()
{
	if (!CachedPlayerCharacter) return;

	bPreviousUseControllerRotationYaw = CachedPlayerCharacter->bUseControllerRotationYaw;
	bPreviousOrientRotationToMovement = CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement;
	
	CachedPlayerCharacter->bUseControllerRotationYaw = false;
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void UKRGA_HeroGuard::UnlockCameraRotation()
{
	if (!CachedPlayerCharacter) return;

	CachedPlayerCharacter->bUseControllerRotationYaw = bPreviousUseControllerRotationYaw;
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = bPreviousOrientRotationToMovement;
}
