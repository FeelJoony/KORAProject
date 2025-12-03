#include "GAS/Abilities/HeroAbilities/KRGA_Grapple.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTag/KREnemyTag.h"
#include "GameplayTag/KRStateTag.h"
#include "Kismet/GameplayStatics.h"

//CharacterMovement->AirControl = 0.35f;

void UKRGA_Grapple::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CachedPlayerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	if (!CachedPlayerCharacter)
	{
		OnAbilityEnd();
	}
	
	bControllerRotationYaw = CachedPlayerCharacter->bUseControllerRotationYaw;
	bRotationToMovement = CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement;

	AController* PlayerController = CachedPlayerCharacter->GetController();
	if (!PlayerController) return;

	CachedPlayerCharacter->SetActorRotation(FRotator(CachedPlayerCharacter->GetActorRotation().Pitch, PlayerController->GetControlRotation().Yaw, CachedPlayerCharacter->GetActorRotation().Roll));
	
	UAbilityTask_PlayMontageAndWait* StartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("StartMontageTask"), StartMontage
	);
	StartMontageTask->OnBlendedIn.AddDynamic(this, &UKRGA_Grapple::LineTrace);
	StartMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::StartMove);
	StartMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::StartMove);
	StartMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::StartMove);
	StartMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::StartMove);
	StartMontageTask->ReadyForActivation();
}

void UKRGA_Grapple::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Grapple::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	CancelGrapple();
}

void UKRGA_Grapple::StartMove()
{
	switch (HitState)
	{
	case EGrappleState::Default:
		{
			CancelGrapple();
		}
		break;
	case EGrappleState::HitEnemy:
		{
			IAbilitySystemInterface* HitPawnASI = Cast<IAbilitySystemInterface>(CachedTargetPawn);
			if (!HitPawnASI) OnAbilityEnd();
			UAbilitySystemComponent* HitPawnASC = HitPawnASI->GetAbilitySystemComponent();
			if (!HitPawnASC) OnAbilityEnd();
			HitPawnASC->AddLooseGameplayTag(KRTAG_STATE_HASCC_STUN);

			OnPullMontageLoop();

			CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			
			GetWorld()->GetTimerManager().SetTimer(
				MoveToTimer,
				this,
				&UKRGA_Grapple::TargetMoveToPlayer,
				0.01f,
				true
			);
			GetWorld()->GetTimerManager().SetTimer(
				MaxGrappleTimer,
				this,
				&UKRGA_Grapple::CancelGrapple,
				EnemyGrappleDuration,
				false
			);
		}
		break;
	case EGrappleState::HitActor:
		{
			OnLoopMontage();
			
			float DistSquard = FVector::DistSquared(CachedPlayerCharacter->GetActorLocation(), GrapPoint);
			float Duration = DistSquard*MoveToSpeed;
			MoveToTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
				this,
				TEXT("MoveToTask"),
				GrapPoint,
				Duration,
				true,
				MOVE_Flying,
				true,
				nullptr,
				ERootMotionFinishVelocityMode::ClampVelocity,
				CachedPlayerCharacter->GetActorLocation(),
				true
			);
			MoveToTask->OnTimedOut.AddDynamic(this, &UKRGA_Grapple::StopPlayerMove);
			MoveToTask->OnTimedOutAndDestinationReached.AddDynamic(this, &UKRGA_Grapple::StopPlayerMove);
			MoveToTask->ReadyForActivation();

			GetWorld()->GetTimerManager().SetTimer(
				MoveToTimer,
				this,
				&UKRGA_Grapple::ApplyCableLocation,
				0.01f,
				true
			);
			
		}
		break;
	}
}

void UKRGA_Grapple::StopPlayerMove()
{
	if (IsValid(MoveToTask))
	{
		MoveToTask->EndTask();
	}
	if (IsValid(LoopMontageTask))
	{
		LoopMontageTask->EndTask();
	}

	CachedPlayerCharacter->bUseControllerRotationYaw = true;
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	if (EndMontageTask==nullptr)
	{
		CachedPlayerCharacter->LaunchCharacter(FVector(0.f,0.f,1.f)*EndLaunchSpeed, true,  true);
		EndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,TEXT("EndMontageTask"),LaunchMontage
		);
		EndMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
		EndMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
		EndMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
		EndMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
		EndMontageTask->ReadyForActivation();
	}
}

void UKRGA_Grapple::LineTrace()
{
	if (!CachedPlayerCharacter) return;

	ApplyCableVisibility(true);
	

	AController* PlayerController = CachedPlayerCharacter->GetController();
	if (!PlayerController) return;

	UCameraComponent* Cam = CachedPlayerCharacter->GetComponentByClass<UCameraComponent>();
	if (!Cam) return;

	FVector StartLocation = CachedPlayerCharacter->GetPawnViewLocation();
	FRotator StartRotation = PlayerController->GetControlRotation();
	FVector EndLocation = StartLocation+(StartRotation.Vector()*TraceRange);

	FHitResult OutHitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CachedPlayerCharacter);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHitResult,StartLocation, EndLocation, ECC_Visibility, Params);
	DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,false,1.f);
	
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), OutHitResult.ImpactPoint, 5.f, 12, FColor::Red, false, 1.f);

		if (AActor* HitActor = Cast<AActor>(OutHitResult.GetActor()))
		{
			if (APawn* HitPawn = Cast<APawn>(HitActor))
			{
				if (IAbilitySystemInterface* HitPawnASI = Cast<IAbilitySystemInterface>(HitPawn))
				{
					if (UAbilitySystemComponent* HitPawnASC = HitPawnASI->GetAbilitySystemComponent())
					{
						if (HitPawnASC->HasMatchingGameplayTag(KRTAG_ENEMY_IMMUNE_GRAPPLE))
						{
							HitState = EGrappleState::HitEnemy;
							
							CachedTargetPawn=HitPawn;

							TargetCapsuleComp = CachedTargetPawn->GetComponentByClass<UCapsuleComponent>();
							if (TargetCapsuleComp)
							{
								GrapPoint=TargetCapsuleComp->GetComponentLocation();
								ApplyCableLocation();
							}
							
							UE_LOG(LogTemp, Display, TEXT("HitEnemy : %s"), *HitPawn->GetName());
							return;
						}
					}
				}
			}
			else
			{
				HitState = EGrappleState::HitActor;

				GrapPoint=OutHitResult.ImpactPoint;
				UGameplayStatics::SpawnDecalAtLocation(
					GetWorld(),
					DecalMaterial,
					FVector(DecalSize),
					OutHitResult.ImpactPoint,
					OutHitResult.ImpactNormal.Rotation(),
					1.f
				);
				ApplyCableLocation();
				return;
			}
		}
	}
	HitState = EGrappleState::Default;
	
	GrapPoint=EndLocation;
	ApplyCableLocation();
}

void UKRGA_Grapple::OnLoopMontage()
{
	if (!LoopMontageTask)
	{
		LoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("LoopMontageTask"),LoopMoveMontage
	);
		LoopMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
		LoopMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
		LoopMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
		LoopMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
		LoopMontageTask->ReadyForActivation();
	}
}

void UKRGA_Grapple::OnPullMontageLoop()
{
	if (!LoopMontageTask)
	{
		LoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("LoopMontageTask"),LoopPullMontage
	);
		LoopMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::OnPullMontageLoop);
		LoopMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::OnPullMontageLoop);
		LoopMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::OnPullMontageLoop);
		LoopMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::OnPullMontageLoop);
		LoopMontageTask->ReadyForActivation();
	}
}

void UKRGA_Grapple::OnAbilityEnd()
{
	CachedPlayerCharacter->bUseControllerRotationYaw = bControllerRotationYaw;
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = bRotationToMovement;
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UKRGA_Grapple::TargetMoveToPlayer()
{
	if (!TargetCapsuleComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_Grapple] Target CapsuleComponent Is Null"))
		return;
	}
	GrapPoint = TargetCapsuleComp->GetComponentLocation();
	ApplyCableLocation();
	
	UCapsuleComponent* PlayerCapsule = CachedPlayerCharacter->GetCapsuleComponent();
	if (!PlayerCapsule)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_Grapple] Player CapsuleComponent Is Null"));
		return;
	}
	FVector PlayerLocation = PlayerCapsule->GetComponentLocation();
	PlayerLocation.Z-=PlayerCapsule->GetScaledCapsuleHalfHeight();

	FVector MoveDirection = (PlayerLocation - GrapPoint).GetSafeNormal();
	FVector MoveOffset = (GetWorld()->GetDeltaSeconds())*MoveDirection*TargetMoveSpeed;
	CachedTargetPawn->AddActorWorldOffset(MoveOffset,true);

	FVector LookDirection = (GrapPoint - PlayerLocation).GetSafeNormal();
	CachedPlayerCharacter->SetActorRotation(FRotator(0.2f,LookDirection.Rotation().Yaw,0.f));

	if (FVector::DistSquared(PlayerLocation,GrapPoint) < StopNealyDistSquared)
	{
		CancelGrapple();
	}
}

void UKRGA_Grapple::CancelGrapple()
{
	ApplyCableVisibility(false);
	
	if (CachedPlayerCharacter)
	{
		CachedPlayerCharacter->StopAnimMontage();
	}

	switch (HitState)
	{
	case EGrappleState::Default:
		{
			OnAbilityEnd();
		}
	break;
	case EGrappleState::HitEnemy:
		{
			IAbilitySystemInterface* HitPawnASI = Cast<IAbilitySystemInterface>(CachedTargetPawn);
			if (!HitPawnASI) OnAbilityEnd();
			UAbilitySystemComponent* HitPawnASC = HitPawnASI->GetAbilitySystemComponent();
			if (!HitPawnASC) OnAbilityEnd();
			HitPawnASC->RemoveLooseGameplayTag(KRTAG_STATE_HASCC_STUN);
			
			if (MoveToTimer.IsValid())
            {
            	GetWorld()->GetTimerManager().ClearTimer(MoveToTimer);
            }
			if (MaxGrappleTimer.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(MaxGrappleTimer);
			}
			OnAbilityEnd();
		}
	break;
	case EGrappleState::HitActor:
		{
			if (MoveToTimer.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(MoveToTimer);
			}
			StopPlayerMove();
		}
	break;
	}
}
