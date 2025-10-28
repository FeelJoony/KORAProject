#include "Animation/KRBaseAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CachedCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (!CachedCharacter)
	{
		UE_LOG(LogTemp,Warning,TEXT("[UKRHeroAnimInstance] Character is null"));
		return;
	}
}

void UKRBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!CachedCharacter)
	{
		UE_LOG(LogTemp,Warning,TEXT("[UKRHeroAnimInstance] Character is null"));
		return;
	}
	GroundSpeed = CachedCharacter->GetVelocity().Size2D();
	bShouldMove = GroundSpeed > 1.0f;

	// bIsFalling 업데이트: Movement Component를 통해 공중 상태를 확인합니다.
	bIsFalling = CachedMoveComponent->IsFalling();
	
}