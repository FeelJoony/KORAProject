#pragma once

#include "CoreMinimal.h"
#include "Animation/KRBaseAnimInstance.h"
#include "KRHeroAnimInstance.generated.h"

UCLASS()
class KORAPROJECT_API UKRHeroAnimInstance : public UKRBaseAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State")
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn State")
	bool bIsLockOn;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn State")
	float LockOnDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn State")
	float LockOnSpeed;	
};
