#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KRHitReactionTypes.generated.h"

/**
 * 히트 방향
 */
UENUM(BlueprintType)
enum class EKRHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right,
	Up,
	Down,
	Any
};

/**
 * 히트 강도
 */
UENUM(BlueprintType)
enum class EKRHitIntensity : uint8
{
	Light, // Combo Light
	Medium, // Combo Medium
	Heavy, // Charge
	Critical // CD Attack
};

/**
 * HitReaction Cue에 전달할 파라미터 구조체
 * GA에서 Cue 호출 시 EffectContext에 담아서 전달
 */
USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRHitReactionCueParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	EKRHitDirection HitDirection = EKRHitDirection::Front;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	EKRHitIntensity HitIntensity = EKRHitIntensity::Light;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	FVector HitLocation = FVector::ZeroVector;

	// 히트 노말 (피격 방향 벡터)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	FVector HitNormal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	FGameplayTag SoundTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	FGameplayTag EffectTag;

	// 이펙트 소켓 (없으면 HitLocation 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	FName EffectSocketName = NAME_None;

	// 넉백 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	float KnockbackDistance = 0.0f;

	// 히트스탑 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitReaction")
	float HitStopDuration = 0.0f;
};

/**
 * HitDirection 계산 유틸리티
 */
UCLASS()
class KORAPROJECT_API UKRHitReactionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 두 액터 사이의 히트 방향 계산
	 * @param Victim 피격자
	 * @param Instigator 공격자
	 * @return 히트 방향 (Front/Back/Left/Right)
	 */
	UFUNCTION(BlueprintPure, Category = "KR|HitReaction")
	static EKRHitDirection CalculateHitDirection(AActor* Victim, AActor* Instigator);

	/**
	 * 히트 방향 벡터로부터 방향 Enum 계산
	 * @param Victim 피격자
	 * @param HitDirection 히트 방향 벡터 (Instigator → Victim)
	 * @return 히트 방향
	 */
	UFUNCTION(BlueprintPure, Category = "KR|HitReaction")
	static EKRHitDirection CalculateHitDirectionFromVector(AActor* Victim, const FVector& HitDirection);

	/**
	 * 히트 방향을 문자열로 변환
	 */
	UFUNCTION(BlueprintPure, Category = "KR|HitReaction")
	static FString HitDirectionToString(EKRHitDirection Direction);

	/**
	 * 히트 강도를 문자열로 변환
	 */
	UFUNCTION(BlueprintPure, Category = "KR|HitReaction")
	static FString HitIntensityToString(EKRHitIntensity Intensity);
};
