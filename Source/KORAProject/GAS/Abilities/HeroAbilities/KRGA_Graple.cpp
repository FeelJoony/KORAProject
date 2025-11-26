#include "GAS/Abilities/HeroAbilities/KRGA_Graple.h"

#include "HeadMountedDisplayTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Characters/KRHeroCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tasks/GameplayTask_WaitDelay.h"

void UKRGA_Graple::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("DashMontageTask"), SelectedMontage
	);
	PlayMontageTask->ReadyForActivation();
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
	StopGraple();
}

void UKRGA_Graple::LineTrace()
{
	FHitResult OutHitResult;
	
	AKRHeroCharacter* PlayerCharacter = Cast<AKRHeroCharacter>(CurrentActorInfo->AvatarActor);
	if (!PlayerCharacter) return;
	
	AController* Controller = PlayerCharacter->GetController();
	if (!Controller) return;

	FVector StartLocation = PlayerCharacter->GetPawnViewLocation();
	
	FRotator ControlRotation = Controller->GetControlRotation();
	FVector EndLocation = StartLocation + (ControlRotation.Vector() * 5000.0f);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerCharacter);
	Params.bTraceComplex = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHitResult,StartLocation,EndLocation,ECollisionChannel::ECC_Camera,Params);
	
	if (bHit)
	{
		// OutHitResult.GetActor()를 사용하여 충돌한 액터에 대한 로직 수행
		UE_LOG(LogTemp, Warning, TEXT("Grapple Hit: %s"), *OutHitResult.GetActor()->GetName());
		ApplyPhysics(OutHitResult, PlayerCharacter);
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
		ACharacter* Character = Cast<ACharacter>(OwnerPawn);
		if (!Character) return;

		TargetLocation = OutHitResult.Location;

		FVector GrapleDirection = (TargetLocation - Character->GetActorLocation()).GetSafeNormal();

		Character->LaunchCharacter(GrapleDirection * StartLaunchSpeed, true, true);

		bIsGrapling = true;

		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
		if (!CharacterMovement) return;
		CharacterMovement->SetMovementMode(MOVE_Flying);
		OriginalGravity=CharacterMovement->GravityScale;
		CharacterMovement->GravityScale = 0.0f;
		
		GetWorld()->GetTimerManager().SetTimer(
			GrapleTimer,
			this,
			&UKRGA_Graple::GoToLocation,
			0.1f
		);
	}
}

void UKRGA_Graple::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_Graple::GoToLocation()
{
	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Character) return;
	
	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
	if (!CharacterMovement) return;

	FVector Direction = (TargetLocation - Character->GetActorLocation()).GetSafeNormal();
	
	CharacterMovement->AddForce(Direction * GrapleSpeed);
	if (FVector::DistSquared(Character->GetActorLocation(), TargetLocation) < FMath::Square(StopGrapleDistance))
	{
		StopGraple();
	}
}

void UKRGA_Graple::StopGraple()
{
	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Character) return;

	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
	if (!CharacterMovement) return;
	CharacterMovement->SetMovementMode(MOVE_Walking);
	CharacterMovement->GravityScale = OriginalGravity;
	
	//컨트롤러가 보는 방향 (or 캐릭터 방향 고려)
	const AController* PlayerController = Character->GetController();
	if (!PlayerController) return;

	FVector Direction = PlayerController->GetControlRotation().Vector();
	Direction.Z += EndDirectionZ; //Direction.Z = 0.5f;
	Direction = Direction.GetSafeNormal();
	
	Character->LaunchCharacter(Direction * EndLaunchSpeed, true, true);
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("DashMontageTask"), EndJumpMontage
	);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Graple::OnAbilityEnd);
	PlayMontageTask->ReadyForActivation();
}
