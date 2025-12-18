#pragma once

#include "CoreMinimal.h"
#include "Animation/KRBaseAnimInstance.h"
#include "KRHeroAnimInstance.generated.h"

class AKRHeroCharacter;
class UKRCharacterMovementComponent;

UCLASS()
class KORAPROJECT_API UKRHeroAnimInstance : public UKRBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimData|Ladder")
	bool bStartFromTop = false;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<AKRHeroCharacter> OwningHeroCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UKRCharacterMovementComponent> CachedKRCMC;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxStateTime = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Ladder")
	bool bIsClimbingLadder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|Ladder")
	float ClimbVelocityZ;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LockOnData")
	//bool bIsLockOn; //태그 관리 고려
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LockOnData")
	//float LockOnDirection;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimData|LockOnData")
	//float LockOnSpeed;
	
	float IdleElapsedTime;
};
