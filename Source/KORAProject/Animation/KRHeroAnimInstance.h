#pragma once

#include "CoreMinimal.h"
#include "Animation/KRBaseAnimInstance.h"
#include "KRHeroAnimInstance.generated.h"

class AKRHeroCharacter;

UCLASS()
class KORAPROJECT_API UKRHeroAnimInstance : public UKRBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<AKRHeroCharacter> OwningHeroCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxStateTime = 5.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LockOnData")
	bool bIsLockOn;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LockOnData")
	float LockOnDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LockOnData")
	float LockOnSpeed;

	float IdleElapsedTime;
};
