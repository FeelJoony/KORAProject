#include "Animation/KRBossAnimInstance.h"

void UKRBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	RecacheOwner();
}

void UKRBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	RecacheOwner();
}

void UKRBossAnimInstance::RecacheOwner()
{
	Super::RecacheOwner();
	if (!CachedBossCharacter || !CachedBossMoveComponent)
	{
		CachedBossCharacter = CachedCharacter;
		CachedBossMoveComponent = CachedMoveComponent;
	}
}
