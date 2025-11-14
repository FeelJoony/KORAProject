#include "Animation/KRBaseAnimInstance.h"
#include "Characters/KRBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CachedCharacter = Cast<AKRBaseCharacter>(TryGetPawnOwner());
	CachedMoveComponent = CachedCharacter->GetCharacterMovement();
}

void UKRBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	GroundSpeed = CachedCharacter->GetVelocity().Size2D();
	
	/*-------------------백승수 수정부분-------------------*/
	bHasAcceleration = CachedMoveComponent->GetCurrentAcceleration().SizeSquared2D()>0.f;
	/*--------------------------------------------------*/
	
	bShouldMove = GroundSpeed > 1.0f;

	// bIsFalling 업데이트: Movement Component를 통해 공중 상태를 확인합니다.
	bIsFalling = CachedMoveComponent->IsFalling();
	
}