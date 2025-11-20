#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KORATypes/KRStructTypes.h"
#include "DataAsset_Weapon.generated.h"


UCLASS(BlueprintType)
class KORAPROJECT_API UDataAsset_Weapon : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Common")
	FKRWeaponCommonData CommonData;
};
