#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KRHitReactionTypes.h"
#include "KRMeleeAttackTypes.generated.h"

class UCameraShakeBase;

/**
 * 공격 형태 타입
 * Shape Trace에 사용할 충돌 형태를 정의
 */
UENUM(BlueprintType)
enum class EKRAttackShapeType : uint8
{
	// 전방 박스 형태 (찌르기, 내려치기)
	Box UMETA(DisplayName = "Box"),

	// 구형 (타격 시점 범위)
	Sphere UMETA(DisplayName = "Sphere"),

	// 원뿔형 (베기 공격) - 여러 Line Trace로 구현
	Cone UMETA(DisplayName = "Cone"),

	// 캡슐형 (회전 공격)
	Capsule UMETA(DisplayName = "Capsule")
};

/**
 * 공격별 설정 구조체
 * 각 공격 콤보마다 다른 범위, 데미지, 피드백을 설정 가능
 */
USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRMeleeAttackConfig
{
	GENERATED_BODY()

	FKRMeleeAttackConfig()
		: AttackShape(EKRAttackShapeType::Cone)
		, AttackRange(200.0f)
		, ConeAngle(90.0f)
		, BoxExtent(50.0f, 100.0f, 100.0f)
		, SphereRadius(100.0f)
		, CapsuleRadius(50.0f)
		, CapsuleHalfHeight(100.0f)
		, bCanHitMultiple(true)
		, MaxHitCount(5)
		, DamageMultiplier(1.0f)
		, HitIntensity(EKRHitIntensity::Medium)
		, WarpDistance(200.0f)
		, bUseHitStop(true)
		, HitStopDuration(0.05f)
		, bUseCameraShake(false)
		, HitCameraShake(nullptr)
	{
	}

	// ─────────────────────────────────────────────────────
	// 히트 감지 설정
	// ─────────────────────────────────────────────────────

	// 공격 형태 (Cone, Box, Sphere, Capsule)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection")
	EKRAttackShapeType AttackShape;

	// 공격 사거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection")
	float AttackRange;

	// 원뿔 각도 (AttackShape == Cone일 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Cone", EditConditionHides, ClampMin = "10.0", ClampMax = "180.0"))
	float ConeAngle;

	// 박스 크기 (AttackShape == Box일 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Box", EditConditionHides))
	FVector BoxExtent;

	// 구체 반지름 (AttackShape == Sphere일 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Sphere", EditConditionHides))
	float SphereRadius;

	// 캡슐 반지름 (AttackShape == Capsule일 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Capsule", EditConditionHides))
	float CapsuleRadius;

	// 캡슐 절반 높이 (AttackShape == Capsule일 때)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Capsule", EditConditionHides))
	float CapsuleHalfHeight;

	// 다중 히트 허용 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection")
	bool bCanHitMultiple;

	// 한 번 휘두르기로 최대 히트 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "bCanHitMultiple", EditConditionHides, ClampMin = "1", ClampMax = "20"))
	int32 MaxHitCount;

	// ─────────────────────────────────────────────────────
	// 데미지 설정
	// ─────────────────────────────────────────────────────

	// 데미지 배율 (무기 기본 데미지에 곱함)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DamageMultiplier;

	// 히트 강도 (HitReaction에 전달)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	EKRHitIntensity HitIntensity;

	// ─────────────────────────────────────────────────────
	// 모션 워핑 설정
	// ─────────────────────────────────────────────────────

	// 타겟 없을 때 전방 이동 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping")
	float WarpDistance;

	// 타겟과 최소 접근 거리 (겹침 방지)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping")
	float MinApproachDistance = 100.0f;

	// 타겟 탐지 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping")
	float TargetSearchRadius = 500.0f;

	// ─────────────────────────────────────────────────────
	// 타격감 설정
	// ─────────────────────────────────────────────────────

	// 히트스톱 사용 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	bool bUseHitStop;

	// 히트스톱 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback",
		meta = (EditCondition = "bUseHitStop", EditConditionHides, ClampMin = "0.01", ClampMax = "0.5"))
	float HitStopDuration;

	// 카메라 쉐이크 사용 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	bool bUseCameraShake;

	// 히트 시 카메라 쉐이크
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback",
		meta = (EditCondition = "bUseCameraShake", EditConditionHides))
	TSubclassOf<UCameraShakeBase> HitCameraShake;
};

/**
 * Melee Attack 유틸리티 라이브러리
 */
UCLASS()
class KORAPROJECT_API UKRMeleeAttackLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 공격 형태를 문자열로 변환
	 */
	UFUNCTION(BlueprintPure, Category = "KR|MeleeAttack")
	static FString AttackShapeToString(EKRAttackShapeType Shape);

	/**
	 * 디버그용 공격 범위 시각화
	 * @param WorldContext 월드 컨텍스트
	 * @param Origin 시작 위치
	 * @param Forward 전방 방향
	 * @param Config 공격 설정
	 * @param Color 디버그 색상
	 * @param Duration 표시 지속 시간
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|MeleeAttack|Debug", meta = (WorldContext = "WorldContextObject"))
	static void DrawDebugAttackShape(
		const UObject* WorldContextObject,
		const FVector& Origin,
		const FVector& Forward,
		const FKRMeleeAttackConfig& Config,
		FColor Color = FColor::Red,
		float Duration = 0.5f
	);
};
