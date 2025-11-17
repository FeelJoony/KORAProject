#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "Interaction/InteractionOption.h"
#include "KRGA_Interact.generated.h"

class UAbilityTask_GrantNearbyInteraction;
class UIndicatorDescriptor;
class UAbilityTask_WaitForInteractableTargets_SingleLineTrace;

UCLASS()
class KORAPROJECT_API UKRGA_Interact : public UKRHeroGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_Interact();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions);
	
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

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
