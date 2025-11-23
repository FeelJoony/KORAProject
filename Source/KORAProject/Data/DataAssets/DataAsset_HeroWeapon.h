#pragma once

#include "CoreMinimal.h"
#include "Data/DataAssets/DataAsset_Weapon.h"
#include "DataAsset_HeroWeapon.generated.h"


UCLASS(BlueprintType)
class KORAPROJECT_API UDataAsset_HeroWeapon : public UDataAsset_Weapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Hero")
	FKRHeroWeaponData HeroWeaponData;
};
