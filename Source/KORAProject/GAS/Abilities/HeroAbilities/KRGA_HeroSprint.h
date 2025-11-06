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
	
protected:
	UFUNCTION()
	void OnBlockStep();
	UFUNCTION()
	void OnAllowJumpStep();
	UFUNCTION()
	void OnAbilityEnd();

	UPROPERTY(EditDefaultsOnly, Category=KRTag)
	FGameplayTag StepAbilityTag;
	UPROPERTY(EditDefaultsOnly, Category=KRTag)
	FGameplayTag JumpAbilityTag;

	UPROPERTY(EditDefaultsOnly, Category=KRTag)
	FGameplayTag CanStepTag;
	UPROPERTY(EditDefaultsOnly, Category=KRTag)
	FGameplayTag CanJumpTag;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> EndDelayTask;
	
	FTimerHandle TimerHandle;
};
