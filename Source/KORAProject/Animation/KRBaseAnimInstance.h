#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KRBaseAnimInstance.generated.h"

class AKRBaseCharacter;
class UCharacterMovementComponent;

UCLASS()
class KORAPROJECT_API UKRBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldMove;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	FVector Velocity;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float Direction;
	
	UPROPERTY()
	TObjectPtr<AKRBaseCharacter> CachedCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CachedMoveComponent = nullptr;

	virtual void RecacheOwner();
};
