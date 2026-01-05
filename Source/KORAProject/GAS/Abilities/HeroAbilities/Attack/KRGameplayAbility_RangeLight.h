#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_RangeAttack.h"
#include "KRGameplayAbility_RangeLight.generated.h"

struct FKRAppliedEquipmentEntry;

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_RangeLight : public UKRGameplayAbility_RangeAttack
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_RangeLight(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	virtual const TArray<TObjectPtr<UAnimMontage>>* GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const override;
};