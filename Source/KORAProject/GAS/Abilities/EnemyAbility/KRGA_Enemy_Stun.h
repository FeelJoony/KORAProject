#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_Enemy_Stun.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class KORAPROJECT_API UKRGA_Enemy_Stun : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;

	UFUNCTION()
	void OnMontageEnded();

	UFUNCTION()
	void ExternalAbilityEnded();

	bool bMontageFinished;

	bool bPendingEnd;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY(EditDefaultsOnly, Category=KRState)
	TObjectPtr<UAnimMontage> StunMontage;

	UPROPERTY(EditDefaultsOnly, Category=KRState)
	FGameplayTag StunAbilityTag;
};
