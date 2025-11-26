#include "GAS/Abilities/HeroAbilities/KRGA_Graple.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Characters/KRHeroCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UKRGA_Graple::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CachedPlayerCharacter = Cast<AKRHeroCharacter>(CurrentActorInfo->AvatarActor);
	if (!CachedPlayerCharacter)
	{
		OnAbilityEnd();
	}

	AController* Controller = CachedPlayerCharacter->GetController();
	if (!Controller) return;
	
	OnLoopMontage();

	TargetRotation = Controller->GetControlRotation();
	
	//Timer로 SetRotation함수 실행
	GetWorld()->GetTimerManager().SetTimer(
		RotatorTimer,
		this,
		&UKRGA_Graple::OnSetRotation,
		0.01f, // 자동으로 deltaTime 간격으로 실행
		true		
	);
	
	LineTrace();
}

void UKRGA_Graple::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Graple::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//케이블 Hidden
	StopGraple();
}

void UKRGA_Graple::LineTrace()
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

		//Cable 가져와서 캐싱하기
		
		ApplyPhysics(OutHitResult, CachedPlayerCharacter);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	
	DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,false,1);
}

void UKRGA_Graple::ApplyPhysics(const FHitResult& OutHitResult, APawn* OwnerPawn)
{
	AActor* HitActor = Cast<AActor>(OutHitResult.GetActor());
	if (!HitActor) return;

	// if (APawn* HitPawn = Cast<APawn>(HitActor)) //생물일 때
	// {
	// 	if (true /* EnemyBase로 Cast되면? Actor Tag가 CanGraple이면? */)
	// 	{
	// 		//적 끌어오는 함수 발동 or 그냥 끌어오기
	// 		//일단 그냥 끌어오기
	// 		
	// 	}
	// 	else
	// 	{
	// 		//못 끌어오기 > Sound 재생 후 End
	// 		
	// 	}
	// }
	// else
	{
		//HitActor로 내가 가기
		if (!CachedPlayerCharacter) return;

		TargetLocation = OutHitResult.Location;

		FVector GrapleDirection = (TargetLocation - CachedPlayerCharacter->GetActorLocation()).GetSafeNormal();

		CachedPlayerCharacter->LaunchCharacter(GrapleDirection * StartLaunchSpeed, true, true);

		StartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("StartMontageTask"), StartMontage
		);
		StartMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Graple::OnMoveToLocation);
		StartMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Graple::OnMoveToLocation);
		StartMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Graple::OnMoveToLocation);
		StartMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Graple::OnMoveToLocation);
		StartMontageTask->ReadyForActivation();
		
	}
}

void UKRGA_Graple::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_Graple::OnLoopMontage()
{
	LoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("LoopMontageTask"), LoopMontage
	);
	LoopMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Graple::OnLoopMontage);
	LoopMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Graple::OnLoopMontage);
	LoopMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Graple::OnLoopMontage);
	LoopMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Graple::OnLoopMontage);
	LoopMontageTask->ReadyForActivation();
	//StopGraple 시 Task 종료 필요
}

void UKRGA_Graple::StopGraple()
{
	if (!CachedPlayerCharacter) return;
	
	const AController* PlayerController = CachedPlayerCharacter->GetController();
	if (!PlayerController) return;

	FVector Direction = PlayerController->GetControlRotation().Vector();
	Direction.Z += 1.f;
	Direction = Direction.GetSafeNormal();
	
	TargetRotation=Direction.Rotation();
	CachedPlayerCharacter->LaunchCharacter(Direction * EndLaunchSpeed, true, true);
	
	EndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("EndMontageTask"), EndMontage
	);
	
	EndMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	EndMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	EndMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	EndMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	EndMontageTask->ReadyForActivation();
}

void UKRGA_Graple::OnMoveToLocation()
{

	//Distance 구한 뒤에 Duration 변경하기
	
	//float Distance = DistSequence(CurrentPlayerCharacter.GetLocation() , TargetLocation.GetLocation()); 
	
	UAbilityTask_ApplyRootMotionMoveToForce* Task =
	UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
		this,
		FName("GrappleToTarget"),
		TargetLocation,
		4.f,
		true,
		EMovementMode::MOVE_Flying,
		true,
		nullptr,
		ERootMotionFinishVelocityMode::ClampVelocity,
		CachedPlayerCharacter->GetActorLocation(),
		true
	);
	if (Task)
	{
		Task->OnTimedOut.AddDynamic(this, &UKRGA_Graple::StopGraple);
		Task->ReadyForActivation();
	}
	
	//도착 못 하고 종료되는 경우에 Task초기화 필요

	OnLoopMontage();
}

void UKRGA_Graple::OnSetRotation()
{
	if (!CachedPlayerCharacter) return;

	FRotator CurrentRot = CachedPlayerCharacter->GetActorRotation();
	
	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

	CachedPlayerCharacter->SetActorRotation(NewRot);
}
