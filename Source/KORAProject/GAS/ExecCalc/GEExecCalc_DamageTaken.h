#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExecCalc_DamageTaken.generated.h"

/**
 * 데미지 계산 ExecCalc
 *
 * 계산 공식:
 * FinalDamage = (BaseDamage × DealDamageMult × CritMultiplier - DefensePower) × TakeDamageMult × (1 - GuardReduction)
 *
 * Source 속성: DealDamageMult, CritChance, CritMulti
 * Target 속성: DefensePower, TakeDamageMult
 *
 * 가드/패리:
 * - KRTAG_STATE_ACTING_GUARD: 가드 상태 (전방 공격 시 GuardConfig.StandardGuardReduction 적용)
 * - KRTAG_STATE_ACTING_PARRY: 패리 타이밍 (전방 공격 시 100% 데미지 감소 + 이벤트 전송)
 */
UCLASS()
class KORAPROJECT_API UGEExecCalc_DamageTaken : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGEExecCalc_DamageTaken();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	/** 공격이 전방에서 온 것인지 확인 */
	bool IsHitFromFront(const FGameplayEffectSpec& Spec, UAbilitySystemComponent* TargetASC) const;

	/** 타겟의 GuardConfig에서 가드 감소율 가져오기 */
	float GetGuardReductionFromConfig(UAbilitySystemComponent* TargetASC) const;
};
