#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/KRMeleeWeapon.h"
#include "KORATypes/KRStructTypes.h"
#include "KRHeroMeleeWeapon.generated.h"

UCLASS()
class KORAPROJECT_API AKRHeroMeleeWeapon : public AKRMeleeWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FKRHeroWeaponData HeroWeaponData;
};
