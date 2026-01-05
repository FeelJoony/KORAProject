#include "Animation/KRHeroAnimInstance.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/KRCharacterMovementComponent.h"
#include "Components/KRPawnExtensionComponent.h"
#include "GameplayTag/KRPlayerTag.h"
#include "GAS/KRAbilitySystemComponent.h"

void UKRHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (CachedCharacter)
	{
		OwningHeroCharacter = Cast<AKRHeroCharacter>(CachedCharacter);
		
		if (OwningHeroCharacter)
		{
			CachedKRCMC = Cast<UKRCharacterMovementComponent>(OwningHeroCharacter->GetCharacterMovement());
		}
	}
}

void UKRHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);


	if (bHasAcceleration)
	{
		IdleElapsedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateTime);
	}
	
	bIsClimbingLadder = false;
	ClimbVelocityZ = 0.f;

	if (CachedKRCMC)
	{
		if (CachedKRCMC->MovementMode == MOVE_Custom && CachedKRCMC->CustomMovementMode == (uint8)EKRMovementMode::Ladder)
		{
			bIsClimbingLadder = true;
			ClimbVelocityZ = CachedKRCMC->Velocity.Z;
		}
	}
		
	/*
	if (bIsLockOn)
	{
		FRotator ControlRotation = CachedCharacter->GetControlRotation();
		FRotator VelocityRotation = CachedCharacter->GetVelocity().Rotation();
        
		LockOnDirection = UKismetMathLibrary::NormalizedDeltaRotator(VelocityRotation, ControlRotation).Yaw;
        
		//CachedMoveComponent->MaxWalkSpeed=LockOnSpeed;
	}
	*/
	
	if (APawn* Pawn = TryGetPawnOwner())
	{
		UpdateGunAnimData(Pawn);
	}
}

void UKRHeroAnimInstance::UpdateGunAnimData(APawn* Pawn)
{
	if (!Pawn) return;
	
	FRotator ControlRot = Pawn->GetControlRotation();
	FRotator ActorRot = Pawn->GetActorRotation();
	FRotator DeltaRot = ControlRot - ActorRot;
	DeltaRot.Normalize(); 

	AimPitch = DeltaRot.Pitch; 
	
	bIsGunMode = false;
	if (UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UKRAbilitySystemComponent* ASC = PawnExtComp->GetKRAbilitySystemComponent())
		{
			bIsGunMode = ASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_GUN);
		}
	}
}