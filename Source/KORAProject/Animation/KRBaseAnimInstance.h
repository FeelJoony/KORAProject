#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KRBaseAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class KORAPROJECT_API UKRBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State")
	bool bShouldMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State")
	bool bIsFalling;

	UPROPERTY()
	TObjectPtr<ACharacter> CachedCharacter;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CachedMoveComponent;
};
