#pragma once

#include "CoreMinimal.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "KORATypes/KRStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "KRHeroMeleeWeapon.generated.h"

UCLASS()
class KORAPROJECT_API AKRHeroMeleeWeapon : public AKRMeleeWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FKRHeroWeaponData HeroWeaponData;

	// UFUNCTION(BlueprintCallable)
	// void AssignGrantAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);
	//
	// UFUNCTION(BlueprintPure)
	// TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;

private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
	
};
