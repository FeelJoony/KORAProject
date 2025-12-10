#pragma once

#include "CoreMinimal.h"
#include "KRGameplayAbility_MeleeBase.h"
#include "KRGameplayAbility_ChargeAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UKRAbilityTask_WaitTick;

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_ChargeAttack : public UKRGameplayAbility_MeleeBase
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_ChargeAttack(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	UFUNCTION()
	void OnChargeTick(float DeltaTime);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();
	
	void StartReleaseMontage(bool bSuccess);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Charge")
	float DamageMulti_Fail;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Charge")
	float DamageMulti_Success;

	UPROPERTY()
	TObjectPtr<UKRAbilityTask_WaitTick> ChargeTickTask;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentChargeMontage;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ChargePlayTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ReleasePlayTask;
	
	// 차지 성공 여부
	bool bIsChargeSuccess;
	// 풀차지가 된 시점인지 (이펙트용)
	bool bIsFullyCharged;
	// 버튼을 누르고 있는 동안만 true
	bool bIsCharging;
	
	float TargetChargeTime;
	float CurrentChargeTime;

	static const FName SECTION_START;
	static const FName SECTION_FAIL;
	static const FName SECTION_SUCCESS;
};
