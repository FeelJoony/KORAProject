#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "KRGA_HeroJump.generated.h"

UCLASS()
class KORAPROJECT_API UKRGA_HeroJump : public UKRHeroGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,	bool bWasCancelled) override;

	UFUNCTION()
	void OnLanded(EMovementMode NewMovementMode);
	UFUNCTION()
	void OnMontageEnded();

	UPROPERTY(EditDefaultsOnly, Category=KRMontage)
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditDefaultsOnly, Category=KRAbility)
	FGameplayTag StepAbilityTag;
};
