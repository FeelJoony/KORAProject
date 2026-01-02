#pragma once

#include "CoreMinimal.h"
#include "KRStaminaTypes.generated.h"

/**
 * 스태미나 회복 상태
 * 상태에 따라 회복 속도가 달라짐
 */
UENUM(BlueprintType)
enum class EStaminaRecoveryState : uint8
{
	Normal,			// 기본 회복 (100%)
	Guarding,		// 가드 중 (33%)
	Sprinting,		// 달리기 중 (0%)
	ActionCooldown	// 동작 후 딜레이 중 (0%)
};

/**
 * 스태미나 비용 설정
 * 각 동작별 스태미나 소모량 정의
 */
USTRUCT(BlueprintType)
struct FKRStaminaCostConfig
{
	GENERATED_BODY()

	// ─────────────────────────────────────────
	// 공격 비용
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Attack")
	float LightAttackCost = 12.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Attack")
	float ChargeAttackBaseCost = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Attack")
	float ChargeAttackPerLevelCost = 5.0f;

	// ─────────────────────────────────────────
	// 회피 비용
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Evade")
	float StepCost = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Evade")
	float DashCost = 18.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Evade")
	float RollCost = 20.0f;

	// ─────────────────────────────────────────
	// 달리기 비용
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "Cost|Sprint")
	float SprintCostPerSecond = 8.0f;

	// ─────────────────────────────────────────
	// 가드 비용
	// ─────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "Cost|Guard", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GuardCostMultiplier = 0.3f;

	// ─────────────────────────────────────────
	// 코어드라이브 비용
	// ─────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "Cost|CoreDrive")
	bool bCoreDriveBurstDrainsAllStamina = true;
};

/**
 * 스태미나 회복 설정
 */
USTRUCT(BlueprintType)
struct FKRStaminaRecoveryConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float BaseRecoveryRate = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GuardingRecoveryMultiplier = 0.33f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float DefaultRecoveryDelay = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float ActionRecoveryDelay = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Recovery", meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float GuardRecoveryDelay = 1.0f;
};

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepletedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangedSignature, float, CurrentStamina, float, MaxStamina);
