#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_RangeAttack.h"
#include "KRGameplayAbility_RangeCharge.generated.h"

class UKRAbilityTask_WaitTick;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_RangeCharge : public UKRGameplayAbility_RangeAttack
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_RangeCharge(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	virtual const TArray<TObjectPtr<UAnimMontage>>* GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const override;

	UFUNCTION()
	void OnChargeTick(float DeltaTime);

	UFUNCTION()
	void OnChargePhaseEnded();

	UFUNCTION()
	void OnReleasePhaseEnded();

	void StartReleaseSequence(bool bIsFullCharge);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "KR|Charge")
	float FullChargeTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|Charge")
	FName SectionName_Start = FName("Start");

	UPROPERTY(EditDefaultsOnly, Category = "KR|Charge")
	FName SectionName_Fire = FName("Fire");

	UPROPERTY(EditDefaultsOnly, Category = "KR|Charge")
	FName SectionName_FireFull = FName("Fire_Full");

private:
	UPROPERTY()
	TObjectPtr<UKRAbilityTask_WaitTick> ChargeTickTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ChargePlayTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ReleasePlayTask;

	bool bIsCharging = false;
	
	bool bIsFullyCharged = false;
	
	float CurrentChargeTime = 0.0f;
};