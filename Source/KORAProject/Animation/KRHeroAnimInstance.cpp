#include "Animation/KRHeroAnimInstance.h"
#include "Characters/KRHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"

void UKRHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (CachedCharacter)
	{
		OwningHeroCharacter = Cast<AKRHeroCharacter>(CachedCharacter);
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