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
	Box UMETA(DisplayName = "Box"),
	Sphere UMETA(DisplayName = "Sphere"),
	Cone UMETA(DisplayName = "Cone"),
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection")
	EKRAttackShapeType AttackShape;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection")
	float AttackRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Cone", EditConditionHides, ClampMin = "10.0", ClampMax = "180.0"))
	float ConeAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Box", EditConditionHides))
	FVector BoxExtent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Sphere", EditConditionHides))
	float SphereRadius;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Capsule", EditConditionHides))
	float CapsuleRadius;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Capsule", EditConditionHides))
	float CapsuleHalfHeight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection")
	bool bCanHitMultiple;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitDetection",
		meta = (EditCondition = "bCanHitMultiple", EditConditionHides, ClampMin = "1", ClampMax = "20"))
	int32 MaxHitCount;

	// ─────────────────────────────────────────────────────
	// 데미지 설정
	// ─────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DamageMultiplier;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	EKRHitIntensity HitIntensity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float KnockbackDistance = 0.0f;

	// ─────────────────────────────────────────────────────
	// 모션 워핑 설정
	// ─────────────────────────────────────────────────────
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping")
	float WarpDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping", meta = (ClampMin = "80.0", ClampMax = "200.0"))
	float MinApproachDistance = 110.0f;

	// ─────────────────────────────────────────────────────
	// 타격감 설정
	// ─────────────────────────────────────────────────────
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	bool bUseHitStop;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback",
		meta = (EditCondition = "bUseHitStop", EditConditionHides, ClampMin = "0.01", ClampMax = "0.5"))
	float HitStopDuration;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	bool bUseCameraShake;
	
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
