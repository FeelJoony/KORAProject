#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "KRGA_Scanning.generated.h"

class AScannerController;

UCLASS()
class KORAPROJECT_API UKRGA_Scanning : public UKRHeroGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Ability")
	void ApplyOutliner();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<AActor*> OverlapActors;
	
private:
	UFUNCTION()
	void StartScan();

	UFUNCTION()
	void SpawnOverlapSphere();
	UFUNCTION()
	void OnEnded();

	UPROPERTY()
	TObjectPtr<ACharacter> CachedCharacter;
	
	UPROPERTY()
	TObjectPtr<AScannerController> CachedScannerController;

	UPROPERTY()
	FTimerHandle ScanTimer;
};
