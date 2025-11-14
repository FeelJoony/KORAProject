#include "Animation/KRBaseAnimInstance.h"
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
		if (!CachedCharacter || !CachedMoveComponent)
		{
			GroundSpeed = 0.f;
			bHasAcceleration = false;
			bShouldMove = false;
			bIsFalling = false;
			return;
		}
		//UE_LOG(LogTemp, Warning, TEXT("KRBaseAnimInstance::NativeUpdateAnimation: CachedCharacter or CachedMoveComponent is nullptr. %s"), *GetName()
	}
	
	GroundSpeed = CachedCharacter->GetVelocity().Size2D();
	bHasAcceleration = CachedMoveComponent->GetCurrentAcceleration().SizeSquared2D()>0.f;
	bShouldMove = GroundSpeed > 1.0f;
	bIsFalling = CachedMoveComponent->IsFalling();
	
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
