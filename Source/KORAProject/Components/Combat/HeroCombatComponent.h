#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"


class AKRHeroMeleeWeapon;

UCLASS()
class KORAPROJECT_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	AKRHeroMeleeWeapon* GetHeroCarriedMeleeWeaponByTag(FGameplayTag InWeaponTag) const;
};
