#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_MeleeBase.h"
#include "KRGA_EnemySlash.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class KORAPROJECT_API UKRGA_EnemySlash : public UKRGameplayAbility_MeleeBase
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageEnded();

	UPROPERTY(EditDefaultsOnly, Category=KRMontage)
	TObjectPtr<UAnimMontage> SlashMontage;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;
	
	UPROPERTY(EditDefaultsOnly, Category=KRAbility)
	FGameplayTag SlashAbilityTag;
};
