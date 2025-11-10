#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/InteractionOption.h"
#include "AbilityTask_GrantNearbyInteraction.generated.h"

class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionAbilityGrantedEvent, FGameplayAbilitySpecHandle, AbilityHandle);

/**
 * Ability task that grants interaction abilities when near interactable objects
 */
UCLASS()
class KORAPROJECT_API UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FInteractionAbilityGrantedEvent OnInteractionGranted;

	UPROPERTY(BlueprintAssignable)
	FInteractionAbilityGrantedEvent OnInteractionRemoved;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(
		UGameplayAbility* OwningAbility,
		float InteractionScanRange,
		float InteractionScanRate);

	virtual void Activate() override;

protected:
	virtual void OnDestroy(bool AbilityEnded) override;

	void QueryInteractables();

	void UpdateInteractionAbility(const TArray<FInteractionOption>& InteractionOptions);

	float InteractionScanRange;
	float InteractionScanRate;

	UPROPERTY()
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	FTimerHandle QueryTimerHandle;
};
