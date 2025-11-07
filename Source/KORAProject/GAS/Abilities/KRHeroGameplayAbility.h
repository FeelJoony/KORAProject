#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRHeroGameplayAbility.generated.h"

class UHeroCombatComponent;
class AKRHeroCharacter;
class AKRHeroController;

UCLASS()
class KORAPROJECT_API UKRHeroGameplayAbility : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "KR|Ability")
	AKRHeroCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "KR|Ability")
	AKRHeroController* GetHeroControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "KR|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

private:
	TWeakObjectPtr<AKRHeroCharacter> CachedKRHeroCharacter;
	TWeakObjectPtr<AKRHeroController> CachedKRHeroController;
};
