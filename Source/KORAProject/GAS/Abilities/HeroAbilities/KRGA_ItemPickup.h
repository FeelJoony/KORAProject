#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_ItemPickup.generated.h"

class AKRQuestItemPickup;

UCLASS()
class KORAPROJECT_API UKRGA_ItemPickup : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_ItemPickup(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	AKRQuestItemPickup* GetItemPickupFromPlayerController(const FGameplayAbilityActorInfo* ActorInfo) const;
};
