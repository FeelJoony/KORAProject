#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_AttackBase.h"
#include "KRGameplayAbility_RangeBase.generated.h"

class AKRRangeWeapon;
class UKREquipmentManagerComponent;
struct FKRAppliedEquipmentEntry;

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_RangeBase : public UKRGameplayAbility_AttackBase
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_RangeBase(const FObjectInitializer& ObjectInitializer);

protected:
	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	FRotator GetFinalAimRotation(float InMaxRange) const;

	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	void FireWeaponActor(const FRotator& InAimRotation);

	AKRRangeWeapon* GetRangeWeapon() const;
	
	UKREquipmentManagerComponent* GetEquipmentManager() const;
	
	virtual const TArray<TObjectPtr<UAnimMontage>>* GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const;

	UAnimMontage* GetMontageFromEquipment(int32 MontageIndex = 0) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Effects")
	FGameplayTag FireTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Combat")
	float MaxRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Cost")
	TSubclassOf<UGameplayEffect> AmmoCostEffectClass;
};