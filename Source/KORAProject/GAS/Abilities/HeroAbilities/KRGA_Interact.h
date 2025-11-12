#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Interaction/InteractionOption.h"
#include "KRGA_Interact.generated.h"

class UAbilityTask_GrantNearbyInteraction;
class UIndicatorDescriptor;
class UAbilityTask_WaitForInteractableTargets_SingleLineTrace;

UCLASS()
class KORAPROJECT_API UKRGA_Interact : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_Interact();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions);
	
	void TriggerInteraction();

protected:
	UPROPERTY()
	TArray<FInteractionOption> CurrentOptions;
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionScanRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bShowLineTraceDebug;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	bool bShowSphereTraceDebug;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
	
};
