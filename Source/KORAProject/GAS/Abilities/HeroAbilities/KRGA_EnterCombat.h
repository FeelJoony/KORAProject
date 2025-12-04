#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_EnterCombat.generated.h"

UCLASS()
class KORAPROJECT_API UKRGA_EnterCombat : public UKRGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
