#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_Enemy_Die.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class KORAPROJECT_API UKRGA_Enemy_Die : public UKRGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;

	UFUNCTION()
	void OrganizeStateTag();

	UFUNCTION()
	void ActivationDie();

	UFUNCTION()
	void OnMontageEnded();

	UFUNCTION()
	void ExternalAbilityEnded();


	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY(EditDefaultsOnly, Category = KRState)
	FGameplayTag DieAbilityTag;
};
