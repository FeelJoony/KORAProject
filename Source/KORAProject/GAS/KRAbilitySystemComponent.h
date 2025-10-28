#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "KRAbilitySystemComponent.generated.h"

UCLASS()
class KORAPROJECT_API UKRAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void ApplyInitialEffects();
	virtual void GiveInitialAbilities();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> BasicAbilities;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> ApplyAbilities;
};
