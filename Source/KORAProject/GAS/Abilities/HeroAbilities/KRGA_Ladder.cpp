#include "GAS/Abilities/HeroAbilities/KRGA_Ladder.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Camera/KRCameraMode.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/KRCameraComponent.h"
#include "Components/KRCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/KRLadderActor.h"
#include "Components/SkeletalMeshComponent.h" 
#include "GameplayTag/KREventTag.h"

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
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (UKRCameraComponent* KRCamera = Character->FindComponentByClass<UKRCameraComponent>())
	{
		if (LadderCameraMode)
		{
			KRCamera->PushCameraMode(LadderCameraMode);
		}
	}
	
	bool bEnterFromTop = (Character->GetActorLocation().Z > TargetLadder->GetTopLocation().Z - 50.f);

	if (bEnterFromTop)
	{
		FVector MountLoc = TargetLadder->GetTopLocation() + (TargetLadder->GetActorForwardVector() * 50.f);
		FRotator MountRot = (-TargetLadder->GetActorForwardVector()).Rotation();
		Character->SetActorLocationAndRotation(MountLoc, MountRot);
		
		KRCMC->StartClimbingLadder(TargetLadder);
		KRCMC->SetLadderMounting(true);
		
		if (MountTopMontage)
		{
			UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MountTopMontage);
			Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
			Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
			Task->ReadyForActivation();
		}
		
		else
		{
			KRCMC->SetLadderMounting(false);
		}
	}
	else
	{
		KRCMC->StartClimbingLadder(TargetLadder);
	}
	
	if (USkeletalMeshComponent* Mesh = Character->GetMesh())
	{
		SavedPrevAnimInstanceClass = Mesh->GetAnimClass();
		if (LadderAnimInstanceClass)
		{
			Mesh->SetAnimInstanceClass(LadderAnimInstanceClass);
		}
	}
	
	UAbilityTask_WaitGameplayEvent* WaitTopTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, KRTAG_EVENT_LADDER_TOP);
	WaitTopTask->EventReceived.AddDynamic(this, &ThisClass::OnReachedTop);
	WaitTopTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitBottomTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, KRTAG_EVENT_LADDER_BOTTOM);
	WaitBottomTask->EventReceived.AddDynamic(this, &ThisClass::OnReachedBottom);
	WaitBottomTask->ReadyForActivation();
	
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
				TWeakObjectPtr<USkeletalMeshComponent> WeakMesh = Mesh;
				TSubclassOf<UAnimInstance> ClassToRestore = SavedPrevAnimInstanceClass;

				if (UWorld* World = GetWorld())
				{
					World->GetTimerManager().SetTimerForNextTick([WeakMesh, ClassToRestore]()
					{
						if (WeakMesh.IsValid() && ClassToRestore)
						{
							WeakMesh->SetAnimInstanceClass(ClassToRestore);
						}
					});
				}
			}
		}
		
		SavedPrevAnimInstanceClass = nullptr;
		
		if (UKRCameraComponent* KRCamera = Character->FindComponentByClass<UKRCameraComponent>())
		{
			if (LadderCameraMode)
			{
				KRCamera->RemoveCameraMode(LadderCameraMode);
			}
		}

		if (UKRCharacterMovementComponent* KRCMC = Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			if (KRCMC->MovementMode == MOVE_Custom && KRCMC->CustomMovementMode == (uint8)EKRMovementMode::Ladder)
			{
				KRCMC->StopClimbingLadder();
			}
			
            KRCMC->SetLadderMounting(false);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Ladder::OnReachedTop(FGameplayEventData Payload)
{
	if (DismountTopMontage)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		UKRCharacterMovementComponent* KRCMC = Character ? Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()) : nullptr;
		
		if (Character && KRCMC)
		{
			KRCMC->SetLadderMounting(true);
			
			if (const AKRLadderActor* Ladder = KRCMC->GetCurrentLadder())
			{
				FVector CorrectLoc = Character->GetActorLocation();
				float IdealZ = Ladder->GetTopLocation().Z - LadderTopHandOffset; 
				CorrectLoc.Z = IdealZ;

				Character->SetActorLocation(CorrectLoc);
			}
		}

		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DismountTopMontage);
		Task->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		Task->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		Task->ReadyForActivation();
	}
	else
	{
		K2_EndAbility();
	}
}

void UKRGA_Ladder::OnReachedBottom(FGameplayEventData Payload)
{
	if (DismountBottomMontage)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		if (UKRCharacterMovementComponent* KRCMC = Character ? Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()) : nullptr)
		{
			KRCMC->SetLadderMounting(true);
		}

		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DismountBottomMontage);
		Task->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		Task->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		Task->ReadyForActivation();
	}
	else
	{
		K2_EndAbility();
	}
}

void UKRGA_Ladder::OnMontageFinished()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (UKRCharacterMovementComponent* KRCMC = Character ? Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()) : nullptr)
	{
		KRCMC->SetLadderMounting(false);
	}
}

void UKRGA_Ladder::OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (PrevMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)EKRMovementMode::Ladder)
	{
		if (IsActive())
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}