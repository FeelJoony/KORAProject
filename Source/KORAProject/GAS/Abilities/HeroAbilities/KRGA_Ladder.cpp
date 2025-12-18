#include "GAS/Abilities/HeroAbilities/KRGA_Ladder.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Camera/KRCameraMode.h"
#include "Components/KRCameraComponent.h"
#include "Components/KRCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/KRLadderActor.h"
#include "Components/SkeletalMeshComponent.h" 
#include "GameplayTag/KREventTag.h"
#include "MotionWarpingComponent.h"

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
	
	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (Mesh)
	{
		SavedPrevAnimInstanceClass = Mesh->GetAnimClass();
		if (LadderAnimInstanceClass)
		{
			Mesh->SetAnimInstanceClass(LadderAnimInstanceClass);
		}
	}

	FVector CharLoc = Character->GetActorLocation();
	FVector TopLoc = TargetLadder->GetTopLocation();
	FVector BottomLoc = TargetLadder->GetBottomLocation(); 
	
	float DistToTop = FVector::DistSquared(CharLoc, TopLoc);
	float DistToBottom = FVector::DistSquared(CharLoc, BottomLoc);
	
	bool bEnterFromTop = (DistToTop < DistToBottom);

	if (bEnterFromTop)
	{
		KRCMC->StartClimbingLadder(TargetLadder);
		KRCMC->SetLadderMounting(true);
    
		if (TargetLadder)
		{
			FVector BaseForward = TargetLadder->GetActorForwardVector();
			FVector CorrectedForward = BaseForward.RotateAngleAxis(TargetLadder->LadderYawOffset, FVector::UpVector);
			FRotator FaceLadderRot = CorrectedForward.Rotation();
			FaceLadderRot.Yaw += 180.f; 

			FVector OrangeCirclePos = TargetLadder->GetTopLocation();
			
			OrangeCirclePos.Z += TopWarpZOffset; 
			OrangeCirclePos += (CorrectedForward * 45.0f); 
			
			if (UMotionWarpingComponent* MWComp = Character->FindComponentByClass<UMotionWarpingComponent>())
			{
				MWComp->AddOrUpdateWarpTargetFromLocationAndRotation(
					FName("LadderTopEntry"), 
					OrangeCirclePos, 
					FaceLadderRot
				);
				
				DrawDebugSphere(GetWorld(), OrangeCirclePos, 15.f, 12, FColor::Blue, false, 5.f);
			}
		}
		
		if (MountTopMontage)
		{
			UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, 
				NAME_None, 
				MountTopMontage
			);
			
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
		FTransform SnapTransform = TargetLadder->GetSnapTransform(Character->GetActorLocation());
		Character->SetActorLocationAndRotation(SnapTransform.GetLocation(), SnapTransform.GetRotation());

		KRCMC->StartClimbingLadder(TargetLadder);
		KRCMC->SetLadderMounting(false);
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
            	
                FVector StartBaseLoc = FVector(CorrectLoc.X, CorrectLoc.Y, Ladder->GetTopLocation().Z);
                FVector CharacterForward = Character->GetActorForwardVector();
                CharacterForward.Z = 0.f;
                CharacterForward.Normalize();
            	
                FVector LandLocation = StartBaseLoc + (CharacterForward * CheckGroundForwardDistance);
            	
                FHitResult HitResult;
                FVector CheckStart = LandLocation + FVector(0.f, 0.f, 100.f);
                FVector CheckEnd = LandLocation - FVector(0.f, 0.f, 100.f);
            	
                FCollisionQueryParams QueryParams;
            	
                QueryParams.AddIgnoredActor(Character);
                QueryParams.AddIgnoredActor(Ladder);

                if (GetWorld()->LineTraceSingleByChannel(HitResult, CheckStart, CheckEnd, ECC_Visibility, QueryParams))
                {
                    LandLocation.Z = HitResult.Location.Z;
                }
            	
                FRotator LandRotation = Character->GetActorRotation();
                LandRotation.Pitch = 0.f;
                LandRotation.Roll = 0.f;
            	
                if (UMotionWarpingComponent* MWComp = Character->FindComponentByClass<UMotionWarpingComponent>())
                {
                    MWComp->AddOrUpdateWarpTargetFromLocationAndRotation(
                        FName("LadderTopExit"), 
                        LandLocation, 
                        LandRotation
                    );
                	
                    DrawDebugSphere(GetWorld(), LandLocation, 15.f, 12, FColor::Green, false, 5.f);
                }
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
	K2_EndAbility();
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