#pragma once

#include "CoreMinimal.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "KRPlayerAbilitySystemComponent.generated.h"

UCLASS()
class KORAPROJECT_API UKRPlayerAbilitySystemComponent : public UKRAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	virtual void ApplyInitialEffects() override;
	virtual void GiveInitialAbilities() override;

	virtual void GiveInputAbilities();

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<class UGameplayAbility>> InitialInputAbilities;

};
