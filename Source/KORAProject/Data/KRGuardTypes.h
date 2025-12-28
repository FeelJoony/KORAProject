#pragma once

#include "CoreMinimal.h"
#include "KRGuardTypes.generated.h"

/**
 * 가드 상태 열거형
 */
UENUM(BlueprintType)
enum class EGuardState : uint8
{
	None,           // 가드 비활성
	ParryWindow,    // 퍼펙트 가드 판정 구간 (입력 직후 ~0.2초)
	StandardGuard,  // 일반 가드 유지
	GuardBroken     // 가드 브레이크
};

/**
 * 가드 설정 구조체
 */
USTRUCT(BlueprintType)
struct FKRGuardConfig
{
	GENERATED_BODY()

	// ─────────────────────────────────────────
	// 퍼펙트 가드
	// ────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "Guard|PerfectGuard", meta = (ClampMin = "0.05", ClampMax = "0.5"))
	float PerfectGuardWindow = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Guard|PerfectGuard", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float PerfectGuardKnockback = 20.0f;

	// ─────────────────────────────────────────
	// 일반 가드
	// ─────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "Guard|StandardGuard", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StandardGuardReduction = 0.7f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Guard|StandardGuard", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GuardStaminaCostMultiplier = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Guard|StandardGuard", meta = (ClampMin = "0.0", ClampMax = "300.0"))
	float StandardGuardKnockback = 100.0f;

	// ─────────────────────────────────────────
	// GreyHP (가드 리게인)
	// ─────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "Guard|GreyHP", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float GreyHPDecayDelay = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Guard|GreyHP", meta = (ClampMin = "0.0", ClampMax = "50.0"))
	float GreyHPDecayRate = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Guard|GreyHP", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GreyHPRecoveryRate = 0.5f;

	// ─────────────────────────────────────────
	// 가드 브레이크
	// ─────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "Guard|GuardBreak", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float GuardBreakDuration = 1.5f;
};

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGuardStateChangedSignature, EGuardState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerfectGuardSuccessSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGuardBrokenSignature);
