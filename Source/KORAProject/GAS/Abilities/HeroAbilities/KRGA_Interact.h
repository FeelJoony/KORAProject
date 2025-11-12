#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Interaction/InteractionOption.h"
#include "KRGA_Interact.generated.h"

class UAbilityTask_GrantNearbyInteraction;
class UIndicatorDescriptor;
class UAbilityTask_WaitForInteractableTargets_SingleLineTrace;
struct FInteractionOption;

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
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
	
	// Lyra : 없음
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	// bool bShowDebug;

	// Lyra : 여기 없음
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	// FCollisionProfileName TraceProfile;

	// Lyra : 바로 실행
	// UPROPERTY()
	// TObjectPtr<UAbilityTask_WaitForInteractableTargets_SingleLineTrace> LineTraceTask;
	// UPROPERTY()
	// TObjectPtr<UAbilityTask_GrantNearbyInteraction> SphereTraceTask;
	
};
