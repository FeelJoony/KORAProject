#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "KRGA_HeroSprint.generated.h"

class UAbilityTask_WaitDelay;

UCLASS()
class KORAPROJECT_API UKRGA_HeroSprint : public UKRHeroGameplayAbility
{
	GENERATED_BODY()
	
public:
	UKRGA_HeroSprint();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,	bool bWasCancelled) override;

	virtual void InputPressed(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	UFUNCTION()
	void OnSprint();
	UFUNCTION()
	void OnAbilityEnd();

	UPROPERTY(EditDefaultsOnly, Category = KRAbility)
	float SprintChargeTime;
	UPROPERTY(EditDefaultsOnly, Category = KRAbility)
	float SprintSpeed;
	UPROPERTY(EditDefaultsOnly, Category = KRAbility)
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category=KRAbility)
	FGameplayTag StepAbilityTag;
	UPROPERTY(EditDefaultsOnly, Category=KRAbility)
	FGameplayTag JumpAbilityTag;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> EndDelayTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> StartSprintTask;
	
	bool bSprintStarted;
	
	FTimerHandle TimerHandle;
};
