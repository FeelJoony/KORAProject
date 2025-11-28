#include "GAS/Abilities/HeroAbilities/KRGA_Grapple.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


//CharacterMovement->AirControl = 0.35f;

void UKRGA_Grapple::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ApplyCableVisibility(true);

	CachedPlayerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	if (!CachedPlayerCharacter)
	{
		OnAbilityEnd();
	}

	AController* Controller = CachedPlayerCharacter->GetController();
	if (!Controller) return;
	
	//OnLoopMontage();

	TargetRotation = Controller->GetControlRotation();

	CachedPlayerCharacter->SetActorRotation(FRotator(CachedPlayerCharacter->GetActorRotation().Pitch, TargetRotation.Yaw, CachedPlayerCharacter->GetActorRotation().Roll));
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	
	// GetWorld()->GetTimerManager().SetTimer(
	// 	RotatorTimer,
	// 	this,
	// 	&UKRGA_Graple::OnSetRotation,
	// 	0.01f, // 자동으로 deltaTime 간격으로 실행
	// 	true		
	// );
	StartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this, TEXT("StartMontageTask"), StartMontage
);
	StartMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::LineTrace);
	StartMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::LineTrace);
	StartMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::LineTrace);
	StartMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::LineTrace);
	StartMontageTask->ReadyForActivation();
	
}

void UKRGA_Grapple::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (EndMontageTask)
	{
		EndMontageTask->EndTask();
	}
	CachedPlayerCharacter->bUseControllerRotationYaw = false;
	CachedPlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Grapple::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//케이블 Hidden
	if (EndMontageTask==nullptr)
	{
		StopGraple();
	}
}

void UKRGA_Grapple::LineTrace()
{
	FHitResult OutHitResult;
	
	if (!CachedPlayerCharacter) return;
	
	AController* Controller = CachedPlayerCharacter->GetController();
	if (!Controller) return;

	FVector StartLocation = CachedPlayerCharacter->GetPawnViewLocation();
	FRotator ControlRotation = Controller->GetControlRotation();
	FVector EndLocation = StartLocation + (ControlRotation.Vector() * TraceRange);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CachedPlayerCharacter);
	Params.bTraceComplex = true; // 정확한 콜리전에 충돌

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHitResult,StartLocation,EndLocation,ECC_Visibility,Params);
	
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), OutHitResult.ImpactPoint, 5.f, 12, FColor::Red, false, 1.f);
		UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			DecalMaterial,
			FVector(20, 20, 20),
			OutHitResult.ImpactPoint,
			OutHitResult.ImpactNormal.Rotation(),
			1.f
		);

		ApplyCableLocation(OutHitResult.ImpactPoint);
		ApplyPhysics(OutHitResult);

	}
	else
	{
		ApplyCableLocation(EndLocation);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	
	DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,false,1);
}

void UKRGA_Grapple::ApplyPhysics(const FHitResult& OutHitResult)
{
	AActor* HitActor = Cast<AActor>(OutHitResult.GetActor());
	if (!HitActor) return;

	if (APawn* HitPawn = Cast<APawn>(HitActor)) //생물일 때
	{
		// UAbilitySystemComponent* TargetASC = HitPawn->GetASC();
		// if (!TargetASC) return;
				
	// if (TargetASC->HasMatchingGameplayTag(EnemyImmuneGrapple))
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("[UKRGA_Grapple] Enemy Hit !!!"));
	// 	//Enemy Stun
	// 	//Enemy SetLocation To Player
	// }
		
	// 	else
	// 	{
	// 		//Boss or OtherPawn : Sound & EndAbility
	// 		//NPC는 충돌 채널에서 제외 필요
	// 	}
		
	}
	else	//Pawn이 아닐 때
	{
		//Hit 위치로 이동
		if (!CachedPlayerCharacter) return;

		TargetLocation = OutHitResult.Location;

		OnMoveToLocation();
	}
}

void UKRGA_Grapple::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_Grapple::OnMoveToLocation()
{
	float Distance = FVector::Dist(CachedPlayerCharacter->GetActorLocation(), TargetLocation);

	float Duration = Distance*MoveToSpeed;
	
	MoveToTask =
	UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		FName("GrappleToTarget"),
		TargetLocation,
		Duration,
		true,
		EMovementMode::MOVE_Flying,
		true,
		nullptr,
		ERootMotionFinishVelocityMode::ClampVelocity,
		CachedPlayerCharacter->GetActorLocation(),
		true
	);
	if (MoveToTask)
	{
		MoveToTask->OnTimedOut.AddDynamic(this, &UKRGA_Grapple::StopGraple);
		MoveToTask->OnTimedOutAndDestinationReached.AddDynamic(this, &UKRGA_Grapple::StopGraple);
		MoveToTask->ReadyForActivation();
	}

	OnLoopMontage();
}

void UKRGA_Grapple::StopGraple()
{
	if (!CachedPlayerCharacter) return;

	if (IsValid(StartMontageTask))
	{
		StartMontageTask->EndTask();
	}
	if (IsValid(LoopMontageTask))
	{
		LoopMontageTask->EndTask();
	}
	if (IsValid(MoveToTask))
	{
		MoveToTask->EndTask();
	}

	ApplyCableVisibility(false);
	
	const AController* PlayerController = CachedPlayerCharacter->GetController();
	if (!PlayerController) return;

	FVector Direction = PlayerController->GetControlRotation().Vector();
	Direction = Direction.GetSafeNormal();
	Direction.Z = 1.f;

	//구르기 몽타주 하는 동안에는 Rotation 동기화
	CachedPlayerCharacter->bUseControllerRotationYaw = true;
	
	TargetRotation = FRotator(0.f, Direction.Rotation().Yaw, 0.f);
	CachedPlayerCharacter->LaunchCharacter(FVector(0.f,0.f,1.f) * EndLaunchSpeed, true, true);
	
	EndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("EndMontageTask"), EndMontage
	);
	
	EndMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
	EndMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
	EndMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
	EndMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::OnAbilityEnd);
	EndMontageTask->ReadyForActivation();
}

void UKRGA_Grapple::OnLoopMontage()
{
	LoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("LoopMontageTask"), LoopMontage
	);
	LoopMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
	LoopMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
	LoopMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
	LoopMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Grapple::OnLoopMontage);
	LoopMontageTask->ReadyForActivation();
}
