#pragma once

#include "CoreMinimal.h"
#include "KRGameplayAbility_MeleeAttack.h"
#include "KRGameplayAbility_ChargeAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UKRAbilityTask_WaitTick;

/**
 * Charge Attack GA (차지 공격)
 * - 버튼 홀드 시 차지
 * - 차지 성공/실패에 따른 다른 공격
 * - 새로운 Shape Trace 기반 히트 감지
 */
UCLASS()
class KORAPROJECT_API UKRGameplayAbility_ChargeAttack : public UKRGameplayAbility_MeleeAttack
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_ChargeAttack(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	/** ChargeAttack 몽타주 배열 반환 */
	virtual const TArray<TObjectPtr<UAnimMontage>>* GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const override;

protected:
	// ─────────────────────────────────────────────────────
	// 차지 설정
	// ─────────────────────────────────────────────────────

	/** 차지 실패 시 데미지 배율 */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Charge")
	float DamageMulti_Fail = 1.0f;

	/** 차지 성공 시 데미지 배율 */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Charge")
	float DamageMulti_Success = 2.0f;

	/** 차지 완료에 필요한 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Charge")
	float TargetChargeTime = 1.0f;

protected:
	UFUNCTION()
	void OnChargeTick(float DeltaTime);

	UFUNCTION()
	void OnChargePhaseCompleted();

	UFUNCTION()
	void OnChargePhaseInterrupted();

	UFUNCTION()
	void OnReleasePhaseCompleted();

	UFUNCTION()
	void OnReleasePhaseInterrupted();

	void StartReleaseMontage(bool bSuccess);

private:
	UPROPERTY()
	TObjectPtr<UKRAbilityTask_WaitTick> ChargeTickTask;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentChargeMontage;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ChargePlayTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ReleasePlayTask;

	// 차지 성공 여부
	bool bIsChargeSuccess = false;
	// 풀차지가 된 시점인지
	bool bIsFullyCharged = false;
	// 버튼을 누르고 있는 동안만 true
	bool bIsCharging = false;

	float CurrentChargeTime = 0.0f;

	static const FName SECTION_START;
	static const FName SECTION_FAIL;
	static const FName SECTION_SUCCESS;
};
