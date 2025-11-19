#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/InteractionOption.h"
#include "AbilityTask_WaitForInteractableTargets_SingleLineTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableObjectsChangedEvent, const TArray<FInteractionOption>&, InteractableOptions);

UCLASS()
class KORAPROJECT_API UAbilityTask_WaitForInteractableTargets_SingleLineTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitForInteractableTargets_SingleLineTrace* WaitForInteractableTargets_SingleLineTrace(UGameplayAbility* OwningAbility,	float InteractionScanRange,	float InteractionScanRate, bool bShowDebug);

protected:
	virtual void OnDestroy(bool AbilityEnded) override;

	void PerformTrace();

	void FixTraceDirection(FVector& TraceStart, float MaxRange, FVector& OutTraceEnd) const;

	void UpdateInteractableOptions(const FInteractionQuery& InteractionQuery, const TScriptInterface<IInteractableTarget>& InteractableTarget);

	//void LineTrace(FHitResult& OutHitReault, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams& InParams);
	
	UPROPERTY()
	FInteractionQuery InteractionQuery;
	
	float LineTraceRange=500.f;
	float LineTraceScanRate=0.1f;
	bool bShowDebug=false;
	
	FTimerHandle TimerHandle;

	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;

	
	// Lyra : 없음
	//
	// void QueryInteractables();
	//
	// UPROPERTY(BlueprintAssignable)
	// FInteractableObjectsChangedEvent OnInteractableObjectsChanged;
	//
	// bool bShowDebug;
	//
	// void PerformTrace();
	//
	// UPROPERTY()
	// FCollisionProfileName TraceProfile;
	//
	// FGameplayAbilityTargetingLocationInfo StartLocation;
};
