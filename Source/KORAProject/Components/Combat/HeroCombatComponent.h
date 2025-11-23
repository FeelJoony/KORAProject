#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "HeroCombatComponent.generated.h"


class AKRWeaponBase;

UCLASS()
class KORAPROJECT_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	AKRWeaponBase* GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const;
	
	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	AKRWeaponBase* GetHeroCurrentEquippedWeapon() const;
	
	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	float GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const;
	
	virtual void OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult) override;
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor, const FHitResult& HitResult) override;
};
