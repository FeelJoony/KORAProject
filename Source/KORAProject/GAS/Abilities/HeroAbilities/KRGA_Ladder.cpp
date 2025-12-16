#include "GAS/Abilities/HeroAbilities/KRGA_Ladder.h"
#include "Components/KRCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/KRLadderActor.h"
#include "Components/SkeletalMeshComponent.h" 

UKRGA_Ladder::UKRGA_Ladder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EKRAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UKRGA_Ladder::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("[GA_Ladder] ActivateAbility Called!"));
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AKRLadderActor* TargetLadder = nullptr;
	if (TriggerEventData && TriggerEventData->Target)
	{
		TargetLadder = Cast<AKRLadderActor>(const_cast<AActor*>(TriggerEventData->Target.Get()));
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	UKRCharacterMovementComponent* KRCMC = Character ? Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()) : nullptr;

	if (!Character || !KRCMC || !TargetLadder)
	{
		UE_LOG(LogTemp, Error, TEXT("[GA_Ladder] Failed: Character(%s), CMC(%s), Target(%s)"), 
			Character ? TEXT("Valid") : TEXT("Null"),
			KRCMC ? TEXT("Valid") : TEXT("Null"),
			TargetLadder ? TEXT("Valid") : TEXT("Null"));
		
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[GA_Ladder] Starting Climb on %s"), *TargetLadder->GetName());
	KRCMC->StartClimbingLadder(TargetLadder);
	
	if (USkeletalMeshComponent* Mesh = Character->GetMesh())
	{
		SavedPrevAnimInstanceClass = Mesh->GetAnimClass();
		
		if (LadderAnimInstanceClass)
		{
			Mesh->SetAnimInstanceClass(LadderAnimInstanceClass);
			UE_LOG(LogTemp, Warning, TEXT("[GA_Ladder] Swapped AnimBP to: %s"), *LadderAnimInstanceClass->GetName());
		}
	}
	
	Character->MovementModeChangedDelegate.AddDynamic(this, &UKRGA_Ladder::OnMovementModeChanged);
}

void UKRGA_Ladder::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	
	if (Character)
	{
		Character->MovementModeChangedDelegate.RemoveDynamic(this, &UKRGA_Ladder::OnMovementModeChanged);
		
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			if (SavedPrevAnimInstanceClass)
			{
				Mesh->SetAnimInstanceClass(SavedPrevAnimInstanceClass);
				UE_LOG(LogTemp, Warning, TEXT("[GA_Ladder] Restored Original AnimBP"));
			}
		}
		
		SavedPrevAnimInstanceClass = nullptr;

		if (UKRCharacterMovementComponent* KRCMC = Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			if (KRCMC->MovementMode == MOVE_Custom && KRCMC->CustomMovementMode == (uint8)EKRMovementMode::Ladder)
			{
				KRCMC->StopClimbingLadder();
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Ladder::OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (PrevMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)EKRMovementMode::Ladder)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}