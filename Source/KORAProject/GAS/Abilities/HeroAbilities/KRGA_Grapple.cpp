#include "GAS/Abilities/HeroAbilities/KRGA_Grapple.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "KRGA_LockOn.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AI/KREnemyPawn.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Camera/CameraComponent.h"
#include "Characters/KREnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTag/KREnemyTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Interaction/GrappleVolume.h"
#include "Enemy/KRAIC_Enemy.h"
#include "GameplayTag/KRAbilityTag.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

//CharacterMovement->AirControl = 0.35f;

void UKRGA_Grapple::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo, 
                                     const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    CachedPlayerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
    if (!CachedPlayerCharacter)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    if (AController* PlayerController = CachedPlayerCharacter->GetController())
    {
        FRotator NewRotation = CachedPlayerCharacter->GetActorRotation();
        NewRotation.Yaw = PlayerController->GetControlRotation().Yaw;
        CachedPlayerCharacter->SetActorRotation(NewRotation);
    }
	
	if (SearchLockOnTarget())
	{
		UAbilitySystemComponent* ASC = ReturnKRASC(CachedTargetPawn);
		if (!ASC) return;

		if (ASC->HasMatchingGameplayTag(KRTAG_ENEMY_IMMUNE_GRAPPLE))
		{
			HitState = EGrappleState::HitEnemy;
		}
		else
		{
			HitState = EGrappleState::HitEnemyFail;
		}		
		BeginStartMontage();
	}
	else
	{
		PerformLineTrace();
	}
}

void UKRGA_Grapple::EndAbility(const FGameplayAbilitySpecHandle Handle, 
                                const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, 
                                bool bReplicateEndAbility, 
                                bool bWasCancelled)
{
    ApplyCableVisibility(false);
	MontageStop(0.25);
	
	CachedTargetPawn = nullptr;

	CachedPlayerCharacter->bUseControllerRotationYaw = false;
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Grapple::InputReleased(const FGameplayAbilitySpecHandle Handle, 
                                   const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);
    CancelGrapple();
}

void UKRGA_Grapple::PerformLineTrace()
{
    if (!CachedPlayerCharacter) return;

    UCameraComponent* Camera = CachedPlayerCharacter->GetComponentByClass<UCameraComponent>();
    if (!Camera) return;

    FVector StartLocation = Camera->GetComponentLocation();
    FRotator StartRotation = Camera->GetComponentRotation();
    FVector EndLocation = StartLocation + (StartRotation.Vector() * TraceRange);
	CachedGrapplePoint = StartLocation + (StartRotation.Vector() * 2000.f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CachedPlayerCharacter);

    GetWorld()->LineTraceSingleByChannel(
        HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
    
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.f);
    DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 12, FColor::Red, false, 1.f);

	JudgeEnumState(HitResult);
	BeginStartMontage();
}

void UKRGA_Grapple::StartMoveLoopMontage()
{
	LoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("LoopMoveMontage"), LoopMoveMontage);
    
	LoopMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::StartMoveLoopMontage);
	LoopMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::StartMoveLoopMontage);
	LoopMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::StartMoveLoopMontage);
	LoopMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::StartMoveLoopMontage);
    
	LoopMontageTask->ReadyForActivation();
}

void UKRGA_Grapple::OnStartCableSetting()
{
	ApplyCableLocation();
	ApplyCableVisibility(true);
}

void UKRGA_Grapple::OnStartMontageFinished()
{
	switch (HitState)
	{
	case EGrappleState::HitEnemy:
		BeginEnemyGrapple();
		break;
		
	case EGrappleState::HitEnemyFail:
		FailEnemyGrapple();
		break;
		
	case EGrappleState::HitPoint:
		BeginActorGrapple();
		break;
        
	case EGrappleState::Default:
	default:
		FinishAbility();
		break;
	}
}

void UKRGA_Grapple::StartPullLoopMontage()
{
	LoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("LoopPullMontage"), LoopPullMontage);
    
	LoopMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::CancelGrapple);
	LoopMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::CancelGrapple);
	LoopMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::CancelGrapple);
	LoopMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::CancelGrapple);
    
	LoopMontageTask->ReadyForActivation();
}

void UKRGA_Grapple::OnActorGrappleFinished()
{
	CleanupActorGrapple();
	BeginLaunchMontage();
}

void UKRGA_Grapple::FinishAbility()
{	
	if (CachedPlayerCharacter)
	{
		CachedPlayerCharacter->bUseControllerRotationYaw = false;
		CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	CleanupAllTasks();
	CleanupAllTimers();

	UKRAbilitySystemComponent* PlayerASC = GetKRAbilitySystemComponentFromActorInfo();
	if (PlayerASC)
	{
		PlayerASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GRAPPLING_SUCCESS);
		PlayerASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_GRAPPLING_FAIL);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_Grapple::BeginStartMontage()
{
	StartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("StartMontage"), StartMontage);
    
	StartMontageTask->OnBlendedIn.AddDynamic(this, &UKRGA_Grapple::OnStartCableSetting);
	StartMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::OnStartMontageFinished);
	StartMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::OnStartMontageFinished);
	StartMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::OnStartMontageFinished);
	StartMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::OnStartMontageFinished);
    
	StartMontageTask->ReadyForActivation();
}

APawn* UKRGA_Grapple::SearchLockOnTarget()
{
	UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponentFromActorInfo();
	if (!KRASC) return nullptr;

	//락온 상태 체크 > 락온 GA에서 대상 return
	if (KRASC->HasMatchingGameplayTag(KRTAG_STATE_ACTING_LOCKON))
	{
		UKRGA_LockOn* LockOnGAInstance = KRASC->GetActiveAbilityByClass<UKRGA_LockOn>();
        if (!LockOnGAInstance) return nullptr;

		AActor* Actor = LockOnGAInstance->GetLockedTarget();
		CachedTargetPawn = Cast<APawn>(Actor);
		if (!CachedTargetPawn) return nullptr;

		TargetCapsuleComp = CachedTargetPawn->GetComponentByClass<UCapsuleComponent>();
		if (!TargetCapsuleComp)
		{
			HitState = EGrappleState::Default;
		}

		return CachedTargetPawn;
	}

	// LockOn 상태가 아니면 nullptr 반환
	return nullptr;
}

AKREnemyPawn* UKRGA_Grapple::IsEnemy(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor) return nullptr;

	AKREnemyPawn* TargetEnemy = Cast<AKREnemyPawn>(HitActor);
	return TargetEnemy;
}

bool UKRGA_Grapple::IsGrappleableEnemy(AKREnemyPawn* TargetEnemy)
{
	UAbilitySystemComponent* ASC = ReturnKRASC(TargetEnemy);
	if (!ASC) return false;

	return ASC->HasMatchingGameplayTag(KRTAG_ENEMY_IMMUNE_GRAPPLE);
}

TArray<AGrappleVolume*> UKRGA_Grapple::ReturnGrappleVolume()
{
	TArray<AGrappleVolume*> Volumes;
	
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape CollisionShape;
	FCollisionQueryParams QueryParams;

	CollisionShape.SetCapsule(50.f,100.f);
	QueryParams.AddIgnoredActor(CachedPlayerCharacter);

	UWorld* World = GetWorld();
	if (!World) return Volumes;

	bool bHit = World->OverlapMultiByChannel(
		OverlapResults, 
		CachedPlayerCharacter->GetActorLocation(), 
		FQuat::Identity,
		ECC_WorldDynamic,
		CollisionShape, 
		QueryParams
	);
	
	// DrawDebugCapsule(
	// 	World,                  // 월드 객체
	// 	CachedPlayerCharacter->GetActorLocation(),      // 캡슐의 중심 위치
	// 	100.f,             // 캡슐의 높이 (실제 캡슐의 절반 높이)
	// 	50.f,                 // 캡슐의 반지름
	// 	FQuat::Identity,
	// 	FColor::Red,             // 색상
	// 	false,                  // bPersistentLines (1프레임만 드로잉)
	// 	3.f,           // LifeTime (3초 동안 표시)
	// 	0,                      // Depth Priority
	// 	5.0f                    // 두께 (Thickness)
	// );

	if (bHit)
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			if (AActor* OverlappedActor = Overlap.GetActor())
			{
				if(AGrappleVolume* GrappleVolume = Cast<AGrappleVolume>(OverlappedActor))
				{
					Volumes.Add(GrappleVolume);
				}
			}
		}
	}
	return Volumes;
}

float UKRGA_Grapple::GrapplePointDistance(const FVector& PointLocation) const
{
	if (!CachedPlayerCharacter) return MAX_flt;
	if (PointLocation == FVector::ZeroVector) return MAX_flt;
	
	return FVector::DistSquared(CachedPlayerCharacter->GetActorLocation(),PointLocation);
}

void UKRGA_Grapple::BeginEnemyGrapple()
{
	if (!ApplyStunToEnemy())
	{
		FinishAbility();
		return;
	}
	
	UKRAbilitySystemComponent* PlayerASC = GetKRAbilitySystemComponentFromActorInfo();
	if (!PlayerASC)
	{
		FinishAbility();
		return;
	}
	
	PlayerASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_GRAPPLING_SUCCESS);

	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

	StartPullLoopMontage();
	
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "CancelGrapple";
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 1001; // 고유값 부여

	ACharacter* TargetCharacter = Cast<ACharacter>(CachedTargetPawn);
	if (TargetCharacter)
	{
		UCharacterMovementComponent* EnemyMoveComp = TargetCharacter->GetCharacterMovement();
		if (EnemyMoveComp)
		{
			EnemyMoveComp->SetMovementMode(MOVE_None); //
			EnemyMoveComp->StopMovementImmediately();
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		MoveToTimer,
		this,
		&UKRGA_Grapple::TickEnemyGrapple,
		0.01f,
		true
	);

	
	float Dist = FVector::Dist(
		CachedPlayerCharacter->GetActorLocation(),
		CachedGrapplePoint);
	float Duration = Dist * TargetMoveInverseSpeed;

	USceneComponent* RootComponent = CachedTargetPawn->GetRootComponent(); 
	if (!RootComponent) CancelGrapple();
		
	UKismetSystemLibrary::MoveComponentTo(
		RootComponent,
		CachedPlayerCharacter->GetActorLocation(),
		FRotator::ZeroRotator,
		true,           // Ease Out
		true,           // Ease In
		Duration,           // 도달 시간
		false,          // Force Shortest Rotation Path
		EMoveComponentAction::Move,
		LatentInfo
	);
	

	GetWorld()->GetTimerManager().SetTimer(
		MaxGrappleTimer,
		this,
		&UKRGA_Grapple::CancelGrapple,
		Duration+0.5f,
		false
	);
	
}

void UKRGA_Grapple::FailEnemyGrapple()
{
	UKRAbilitySystemComponent* PlayerASC = GetKRAbilitySystemComponentFromActorInfo();
	if (!PlayerASC)
	{
		FinishAbility();
		return;
	}
	
	PlayerASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_GRAPPLING_SUCCESS);
	FinishAbility();
}

FVector UKRGA_Grapple::ReturnGrapplePointLocation()
{
	CachedGrapplePoint=FVector::ZeroVector;
	
	if (!CachedPlayerCharacter) return CachedGrapplePoint;
	
	TArray<AGrappleVolume*> Volumes = ReturnGrappleVolume();
	if (Volumes.Num() > 0)
	{
		for (AGrappleVolume* Volume : Volumes)
		{
			if (AActor* GrapplePoint = Volume->CachedGrapplePoint)
			{
				if (CachedGrapplePoint == FVector::ZeroVector)
				{
					CachedGrapplePoint = GrapplePoint->GetActorLocation();
				}
				else
				{
					if (GrapplePointDistance(GrapplePoint->GetActorLocation()) < GrapplePointDistance(CachedGrapplePoint))
					{
						CachedGrapplePoint = GrapplePoint->GetActorLocation();
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("Now GrapplePoint : %s"), *GrapplePoint->GetName());
			}
		}
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("GrapplePoint PointLocation : %f, %f, %f"), CachedGrapplePoint.X, CachedGrapplePoint.Y, CachedGrapplePoint.Z);
	return CachedGrapplePoint;
}

bool UKRGA_Grapple::ApplyStunToEnemy()
{
	UKRAbilitySystemComponent* ASC = ReturnKRASC(CachedTargetPawn);
	if (!ASC) return false;

	ASC->AddLooseGameplayTag(KRTAG_STATE_HASCC_STUN);
	return true;
}

void UKRGA_Grapple::JudgeEnumState(FHitResult& HitResult)
{
	if (AKREnemyPawn* TargetEnemy = IsEnemy(HitResult))
	{
		TargetCapsuleComp = TargetEnemy->GetComponentByClass<UCapsuleComponent>();
		if (!TargetCapsuleComp)
		{
			HitState = EGrappleState::Default;
			return;;
		}
		CachedTargetPawn = TargetEnemy;
		
		// AKRAIC_Enemy* EnemyAIC = Cast<AKRAIC_Enemy>(CachedTargetPawn->GetController());
		// if (!IsValid(EnemyAIC)) return;
		// EnemyAIC->SetAttackTarget(GetAvatarActorFromActorInfo());
		
		if (IsGrappleableEnemy(TargetEnemy))
		{
			HitState = EGrappleState::HitEnemy;
		}
		else
		{
			HitState = EGrappleState::HitEnemyFail;
		}
	}
	else
	{
		if (ReturnGrapplePointLocation() == FVector::ZeroVector)
		{
			HitState = EGrappleState::Default;
			
			UCameraComponent* Camera = CachedPlayerCharacter->GetComponentByClass<UCameraComponent>();
			if (!Camera) return;

			FVector StartLocation = Camera->GetComponentLocation();
			FRotator StartRotation = Camera->GetComponentRotation();
			CachedGrapplePoint = StartLocation + (StartRotation.Vector() * 2000.f);
		}
		else
		{
			HitState = EGrappleState::HitPoint;
		}
	}
}

void UKRGA_Grapple::RemoveStunFromEnemy()
{
    UKRAbilitySystemComponent* ASC = ReturnKRASC(CachedTargetPawn);
    if (!ASC) return;

    ASC->RemoveLooseGameplayTag(KRTAG_STATE_HASCC_STUN);
}

void UKRGA_Grapple::TickEnemyGrapple()
{
    if (!TargetCapsuleComp || !CachedTargetPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Grapple] Invalid target components"));
        CancelGrapple();
        return;
    }

    CachedGrapplePoint = TargetCapsuleComp->GetComponentLocation();
    ApplyCableLocation();

    UCapsuleComponent* PlayerCapsule = CachedPlayerCharacter->GetCapsuleComponent();
    if (!PlayerCapsule) return;

    FVector PlayerLocation = PlayerCapsule->GetComponentLocation();
    PlayerLocation.Z -= PlayerCapsule->GetScaledCapsuleHalfHeight();
	
    FVector LookDirection = (CachedGrapplePoint - PlayerLocation).GetSafeNormal();
    CachedPlayerCharacter->SetActorRotation(FRotator(0.2f, LookDirection.Rotation().Yaw, 0.f));

    if (FVector::DistSquared(PlayerLocation, CachedGrapplePoint) < StopNealyDistSquared)
    {
        CancelGrapple();
    }
}

void UKRGA_Grapple::CleanupEnemyGrapple()
{
    RemoveStunFromEnemy();
	if (LoopMontageTask && LoopMontageTask->IsActive())
	{
		LoopMontageTask->EndTask();
	}
    GetWorld()->GetTimerManager().ClearTimer(MoveToTimer);
    GetWorld()->GetTimerManager().ClearTimer(MaxGrappleTimer);
	CachedPlayerCharacter->StopAnimMontage(LoopPullMontage);
	BeginEndPullMontage();
}

void UKRGA_Grapple::BeginEndPullMontage()
{
	if (!CachedPlayerCharacter || !EndPullMontage)
	{
		FinishAbility();
		return;
	}

	if (EndMontageTask && EndMontageTask->IsActive())
	{
		FinishAbility();
		return;
	}

	EndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("EndPullMontage"), EndPullMontage);
    
	EndMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
	EndMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
	EndMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
	EndMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
    
	EndMontageTask->ReadyForActivation();
}

void UKRGA_Grapple::BeginActorGrapple()
{
	StartMoveLoopMontage();

	float Dist = FVector::Dist(
		CachedPlayerCharacter->GetActorLocation(),
		CachedGrapplePoint);
	float Duration = Dist * MoveToInverseSpeed;
	//UE_LOG(LogTemp,Warning,TEXT("[GA_Grapple] DistSquared : %f, Duration : %f"),Dist,Duration);

	MoveToTask = UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		TEXT("MoveToTask"),
		CachedGrapplePoint,
		Duration,
		true,
		MOVE_Flying,
		true,
		nullptr,
		ERootMotionFinishVelocityMode::ClampVelocity,
		CachedPlayerCharacter->GetActorLocation(),
		true
	);

	MoveToTask->OnTimedOut.AddDynamic(this, &UKRGA_Grapple::OnActorGrappleFinished);
	MoveToTask->OnTimedOutAndDestinationReached.AddDynamic(this, &UKRGA_Grapple::OnActorGrappleFinished);
	MoveToTask->ReadyForActivation();

	GetWorld()->GetTimerManager().SetTimer(
		MoveToTimer,
		this,
		&UKRGA_Grapple::TickActorGrappleMovement,
		0.01f,
		true
	);

	UAbilityTask_WaitDelay* DurationTask = UAbilityTask_WaitDelay::WaitDelay(this, Duration);
	DurationTask->OnFinish.AddDynamic(this, &UKRGA_Grapple::OnActorGrappleFinished);
	DurationTask->ReadyForActivation();
}

void UKRGA_Grapple::TickActorGrappleMovement()
{
	ApplyCableLocation();

	//UE_LOG(LogTemp, Warning, TEXT("[GA_Grapple] GrapPoint : %f, %f, %f"),GrapPoint.X,GrapPoint.Y,GrapPoint.Z);
	
	FVector PlayerLocation = CachedPlayerCharacter->GetActorLocation();
	FVector LookDirection = (CachedGrapplePoint - PlayerLocation).GetSafeNormal();
	CachedPlayerCharacter->SetActorRotation(FRotator(0.2f, LookDirection.Rotation().Yaw, 0.f));
}

void UKRGA_Grapple::CleanupActorGrapple()
{
	if (MoveToTask && MoveToTask->IsActive())
	{
		MoveToTask->EndTask();
	}
    
	if (LoopMontageTask && LoopMontageTask->IsActive())
	{
		LoopMontageTask->EndTask();
	}

	GetWorld()->GetTimerManager().ClearTimer(MoveToTimer);

	if (CachedPlayerCharacter)
	{
		CachedPlayerCharacter->StopAnimMontage(LoopMoveMontage);
	}

	CachedPlayerCharacter->bUseControllerRotationYaw = true;
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void UKRGA_Grapple::BeginLaunchMontage()
{
	if (EndMontageTask && EndMontageTask->IsActive())
	{
		FinishAbility();
		return;
	}

	CachedPlayerCharacter->LaunchCharacter(
		FVector::UpVector * EndLaunchSpeed, 
		true, 
		true
	);

	EndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("LaunchMontage"), LaunchMontage);
    
	EndMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
	EndMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
	EndMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
	EndMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::FinishAbility);
    
	EndMontageTask->ReadyForActivation();
}

void UKRGA_Grapple::CancelGrapple()
{
	ApplyCableVisibility(false);
	
	switch (HitState)
	{
	case EGrappleState::HitEnemy:
		CleanupEnemyGrapple();
		break;
        
	case EGrappleState::HitPoint:
		CleanupActorGrapple();
		BeginLaunchMontage();
		return;
        
	case EGrappleState::Default:
	default:
		break;
	}

	FinishAbility();
}

void UKRGA_Grapple::CleanupAllTasks()
{
	if (StartMontageTask && StartMontageTask->IsActive())
	{
		StartMontageTask->EndTask();
	}
    
	if (LoopMontageTask && LoopMontageTask->IsActive())
	{
		LoopMontageTask->EndTask();
	}
    
	if (MoveToTask && MoveToTask->IsActive())
	{
		MoveToTask->EndTask();
	}
	
	if (EndMontageTask && EndMontageTask->IsActive())
	{
		EndMontageTask->EndTask();
	}
}

void UKRGA_Grapple::CleanupAllTimers()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(MoveToTimer);
	World->GetTimerManager().ClearTimer(MaxGrappleTimer);
}

void UKRGA_Grapple::PlayGrappleSound(USoundWave* SoundWave)
{
	UWorld* World = GetWorld();
	if (!World || !SoundWave) return;
	
	UGameplayStatics::PlaySound2D(World,SoundWave);
}

UKRAbilitySystemComponent* UKRGA_Grapple::ReturnKRASC(APawn* APawn)
{
	if (!APawn) return nullptr;
	
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(APawn);
	if (!ASI) return nullptr;
	
	if (UKRAbilitySystemComponent* ASC = Cast<UKRAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
	{
		return ASC;
	}
	
	return nullptr;	
}
