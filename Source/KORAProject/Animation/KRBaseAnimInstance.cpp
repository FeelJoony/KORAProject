#include "Animation/KRBaseAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Characters/KRBaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	RecacheOwner();
}

void UKRBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (UNLIKELY(!CachedCharacter || !CachedMoveComponent))
	{
		RecacheOwner();
		// if (!CachedCharacter || !CachedMoveComponent)
		// {
		// 	GroundSpeed = 0.f;
		// 	bHasAcceleration = false;
		// 	bShouldMove = false;
		// 	bIsFalling = false;
		// 	return;
		// }
		//UE_LOG(LogTemp, Warning, TEXT("KRBaseAnimInstance::NativeUpdateAnimation: CachedCharacter or CachedMoveComponent is nullptr. %s"), *GetName()
	}
	
	GroundSpeed = CachedCharacter->GetVelocity().Size2D();
	bHasAcceleration = !CachedMoveComponent->GetCurrentAcceleration().IsNearlyZero();
	bShouldMove = GroundSpeed > 5.0f;
	bIsFalling = CachedMoveComponent->IsFalling();
	Velocity = CachedCharacter->GetVelocity();
	Direction = UKismetAnimationLibrary::CalculateDirection(
		Velocity,
		CachedCharacter->GetActorRotation()
	);
}

void UKRBaseAnimInstance::RecacheOwner()
{
	APawn* PawnOwner = TryGetPawnOwner();
	CachedCharacter = PawnOwner ? Cast<AKRBaseCharacter>(PawnOwner) : nullptr;

	if (CachedCharacter)
	{
		CachedMoveComponent = CachedCharacter->GetCharacterMovement();
		return;
	}
	
	if (const ACharacter* AnyCharacter = PawnOwner ? Cast<ACharacter>(PawnOwner) : nullptr)
	{
		CachedMoveComponent = AnyCharacter->GetCharacterMovement();
	}
	else
	{
		CachedMoveComponent = nullptr;
	}
}
