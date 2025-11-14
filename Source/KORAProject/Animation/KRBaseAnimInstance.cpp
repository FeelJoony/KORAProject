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
	/*-------------------백승수 수정부분-------------------*/
	bHasAcceleration = CachedMoveComponent->GetCurrentAcceleration().SizeSquared2D()>0.f;
	/*--------------------------------------------------*/
	bShouldMove = GroundSpeed > 1.0f;
	// bIsFalling 업데이트: Movement Component를 통해 공중 상태를 확인합니다.
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
