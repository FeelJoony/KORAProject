#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/KRWeaponBase.h"
#include "KRMeleeWeapon.generated.h"

class UBoxComponent;

UCLASS()
class KORAPROJECT_API AKRMeleeWeapon : public AKRWeaponBase
{
	GENERATED_BODY()

public:
	AKRMeleeWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	TObjectPtr<UBoxComponent> WeaponCollisionBox;

public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const {return WeaponCollisionBox;}
};
