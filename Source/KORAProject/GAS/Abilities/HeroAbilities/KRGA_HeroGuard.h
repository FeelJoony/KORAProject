#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_HeroGuard.generated.h"

class UAbilityTask_WaitInputRelease;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class KORAPROJECT_API UKRGA_HeroGuard : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_HeroGuard(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardBreakMontage;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> InputReleaseTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> GuardBrokenEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ParrySuccessEventTask;

	UFUNCTION()
	void OnGuardInputReleased(float TimeHeld);

	UFUNCTION()
	void OnGuardBrokenEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnParrySuccessEventReceived(FGameplayEventData Payload);
	
	void StartGuardState();
	void StopGuardState();


};