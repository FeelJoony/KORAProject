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
	AKRHeroMeleeWeapon* GetHeroCarriedMeleeWeaponByTag(FGameplayTag InWeaponTag) const;
};
