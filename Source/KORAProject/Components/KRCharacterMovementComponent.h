#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KRCharacterMovementComponent.generated.h"


class UKRAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API UKRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	mutable TWeakObjectPtr<UKRAbilitySystemComponent> CachedASC;
};
