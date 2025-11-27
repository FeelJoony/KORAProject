#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_HeroSprint.generated.h"

class UAbilityTask_WaitDelay;

UCLASS()
class KORAPROJECT_API UKRGA_HeroSprint : public UKRGameplayAbility
{
	GENERATED_BODY()
	
public:	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,	bool bWasCancelled) override;

	virtual void InputPressed(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UFUNCTION()
	void OnAbilityEnd();
	
protected:
	UFUNCTION()
	void OnSprint();

	UPROPERTY(EditDefaultsOnly, Category = KRAbility)
	float SprintChargeTime = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = KRAbility)
	float SprintSpeed = 800.f;
	UPROPERTY(EditDefaultsOnly, Category = KRAbility)
	float WalkSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category=KRAbility)
	FGameplayTag StepAbilityTag;
	UPROPERTY(EditDefaultsOnly, Category=KRAbility)
	FGameplayTag JumpAbilityTag;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> EndDelayTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> StartSprintTask;
	
	bool bSprintStarted = false;
	
	FTimerHandle TimerHandle;
};
