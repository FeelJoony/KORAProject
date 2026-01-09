#pragma once

#include "CoreMinimal.h"
#include "Animation/KRBaseAnimInstance.h"
#include "KRBossAnimInstance.generated.h"

UCLASS()
class KORAPROJECT_API UKRBossAnimInstance : public UKRBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	TObjectPtr<class ACharacter> CachedBossCharacter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	TObjectPtr<class UCharacterMovementComponent> CachedBossMoveComponent = nullptr;

	virtual void RecacheOwner() override;
};
