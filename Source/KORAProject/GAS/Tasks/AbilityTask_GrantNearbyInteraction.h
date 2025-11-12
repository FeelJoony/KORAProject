#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_GrantNearbyInteraction.generated.h"

class UGameplayAbility;
struct FInteractionOption;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionAbilityGrantedEvent, const TArray<FInteractionOption>&, InteractableOptions);

/**
 * Ability task that grants interaction abilities when near interactable objects
 */
UCLASS()
class KORAPROJECT_API UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FInteractionAbilityGrantedEvent InteractionChanged; //IntaractionObjects가 변하면 이벤트 실행

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(
		UGameplayAbility* OwningAbility,
		bool ShowDebugInfo,
		float InteractionScanRange,
		float InteractionScanRate);

	virtual void Activate() override;

protected:
	virtual void OnDestroy(bool AbilityEnded) override;

	void QueryInteractables();

	//Options의 ItemInteractionGA를 Task Owner ASC에 대입
	void UpdateAllTargetOptions(TArray<FInteractionOption>& Options);

	void CleanOutRangeAbility(const TMap<FObjectKey, FGameplayAbilitySpecHandle>& AbilitiesToKeep);

	void SortOptions(TArray<FInteractionOption>& Options);

	float CompareDistance(const FInteractionOption& Option) const;

	//행위 주체 필터링 후 Option에 대입
	// void UpdateInteractableOptions(const FInteractionQuery& InInteractionQuery,
	//                                const TScriptInterface<IInteractableTarget>& InteractableTarget);
	
	bool ShowDebugInfo;
	float InteractionScanRange;
	float InteractionScanRate;

	FTimerHandle QueryTimerHandle;

	TArray<FInteractionOption> CurrentOptions;
	
	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
