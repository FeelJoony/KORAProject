#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KREnemyGameplayAbility.generated.h"

class AKREnemyCharacter;
class UEnemyCombatComponent;

UCLASS()
class KORAPROJECT_API UKREnemyGameplayAbility : public UKRGameplayAbility
{
	GENERATED_BODY()

/*public:
	UFUNCTION(BlueprintPure, Category = "KR|Ability")
	AKREnemyCharacter* GetEnemyCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "KR|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

private:
	TWeakObjectPtr<AKREnemyCharacter> CachedKREnemyCharacter;*/
};
