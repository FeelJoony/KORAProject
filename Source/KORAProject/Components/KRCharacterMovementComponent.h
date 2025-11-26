#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KRCharacterMovementComponent.generated.h"


UCLASS()
class KORAPROJECT_API UKRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;
};
