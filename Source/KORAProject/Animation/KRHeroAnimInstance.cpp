#include "Animation/KRHeroAnimInstance.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (CachedCharacter)
	{
		CachedMoveComponent = CachedCharacter->GetCharacterMovement();
        if (!CachedMoveComponent)
        {
        	UE_LOG(LogTemp,Warning,TEXT("[UKRHeroAnimInstance] Character is null"));
        	return;
        }
	}
}

void UKRHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!CachedMoveComponent)
	{
		UE_LOG(LogTemp,Warning,TEXT("[UKRHeroAnimInstance] Character is null"));
		return;
	}
	
	Direction = UKismetAnimationLibrary::CalculateDirection(
			CachedCharacter->GetVelocity(),
			CachedCharacter->GetActorRotation()
	);
	
	if (bIsLockOn)
	{
		FRotator ControlRotation = CachedCharacter->GetControlRotation();
		FRotator VelocityRotation = CachedCharacter->GetVelocity().Rotation();
        
		LockOnDirection = UKismetMathLibrary::NormalizedDeltaRotator(VelocityRotation, ControlRotation).Yaw;
        
		//CachedMoveComponent->MaxWalkSpeed=LockOnSpeed;
	}
}