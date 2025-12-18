#include "Animation/KRHeroAnimInstance.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/KRCharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Interaction/KRLadderActor.h"

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
}