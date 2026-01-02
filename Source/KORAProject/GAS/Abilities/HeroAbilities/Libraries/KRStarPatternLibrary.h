#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KRStarPatternLibrary.generated.h"

USTRUCT(BlueprintType)
struct FKRStarPatternData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Star Pattern")
	TArray<FVector> StarPoints;

	UPROPERTY(BlueprintReadOnly, Category = "Star Pattern")
	TArray<FVector> DrawOrderPoints;
	UPROPERTY(BlueprintReadOnly, Category = "Star Pattern")
	FVector CenterLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Star Pattern")
	float Radius;
	
	UPROPERTY(BlueprintReadOnly, Category = "Star Pattern")
	TArray<FVector> LineStartPoints;
	UPROPERTY(BlueprintReadOnly, Category = "Star Pattern")
	TArray<FVector> LineEndPoints;
};

/**
 * 별 모양 패턴 생성 유틸리티
 *
 * 사용 예시:
 * - 타겟 주변에 별 모양 대시 경로 계산
 * - Niagara 이펙트에 별 모양 라인 데이터 전달
 */
UCLASS()
class KORAPROJECT_API UKRStarPatternLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 타겟 주변에 별 모양 포인트 생성
	 * @param Center 별의 중심점 (보통 타겟 위치)
	 * @param Radius 별의 반지름 (중심에서 꼭짓점까지 거리)
	 * @param UpVector 별이 향할 방향 (기본: Z축, 수평 별은 FVector::UpVector)
	 * @param StartAngleOffset 시작 각도 오프셋 (도, 기본 0 = 위쪽 시작)
	 * @return 별 패턴 데이터
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarPattern")
	static FKRStarPatternData GenerateStarPattern(
		FVector Center,
		float Radius,
		FVector UpVector = FVector::UpVector,
		float StartAngleOffset = 0.0f
	);

	/**
	 * 수평면에 별 모양 생성 (바닥에 그려지는 별)
	 * @param Center 중심점
	 * @param Radius 반지름
	 * @param ForwardVector 별의 첫 꼭짓점이 향할 방향
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarPattern")
	static FKRStarPatternData GenerateHorizontalStarPattern(
		FVector Center,
		float Radius,
		FVector ForwardVector
	);

	/**
	 * 타겟 액터 주변에 별 모양 생성
	 * @param TargetActor 중심이 될 액터
	 * @param Radius 반지름
	 * @param HeightOffset 높이 오프셋 (액터 위치 기준)
	 * @param bHorizontal true: 바닥에 수평, false: 액터를 향해 수직
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarPattern")
	static FKRStarPatternData GenerateStarPatternAroundActor(
		AActor* TargetActor,
		float Radius,
		float HeightOffset = 0.0f,
		bool bHorizontal = true
	);

	/**
	 * 별 패턴의 특정 인덱스 포인트 가져오기 (대시 순서용)
	 * @param PatternData 별 패턴 데이터
	 * @param Index 인덱스 (0-4, 별 그리는 순서)
	 */
	UFUNCTION(BlueprintPure, Category = "KR|StarPattern")
	static FVector GetStarPointByDrawOrder(const FKRStarPatternData& PatternData, int32 Index);

	/**
	 * 두 포인트 사이의 방향 벡터 계산
	 */
	UFUNCTION(BlueprintPure, Category = "KR|StarPattern")
	static FVector GetDirectionBetweenPoints(const FKRStarPatternData& PatternData, int32 FromIndex, int32 ToIndex);

	/**
	 * 별 라인의 총 길이 계산
	 */
	UFUNCTION(BlueprintPure, Category = "KR|StarPattern")
	static float GetTotalStarLineLength(const FKRStarPatternData& PatternData);

	/**
	 * 특정 라인 세그먼트의 길이
	 */
	UFUNCTION(BlueprintPure, Category = "KR|StarPattern")
	static float GetLineSegmentLength(const FKRStarPatternData& PatternData, int32 SegmentIndex);

	/**
	 * 10포인트 별 패턴 생성 (외곽 5개 + 내부 교차점 5개)
	 * 완전한 별 윤곽선을 그리는 경로: 외곽→내부→외곽→내부... 시계방향
	 * @param Center 별의 중심점
	 * @param OuterRadius 외곽 꼭짓점까지 거리 (별 끝)
	 * @param InnerRadius 내부 교차점까지 거리 (기본: 외곽의 38%)
	 * @param UpVector 별이 향할 방향
	 * @param StartAngleOffset 시작 각도 오프셋
	 * @return 10포인트 별 패턴 데이터
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarPattern")
	static FKRStarPatternData Generate10PointStarPattern(
		FVector Center,
		float OuterRadius,
		float InnerRadius = 0.0f,
		FVector UpVector = FVector::UpVector,
		float StartAngleOffset = 0.0f
	);

	/**
	 * 타겟 액터 주변에 10포인트 별 패턴 생성
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarPattern")
	static FKRStarPatternData Generate10PointStarPatternAroundActor(
		AActor* TargetActor,
		float OuterRadius,
		float HeightOffset = 0.0f,
		bool bHorizontal = true
	);

private:
	// 정오각형 꼭짓점 계산 (0-4)
	static TArray<FVector> CalculatePentagonPoints(FVector Center, float Radius, FVector UpVector, float StartAngle);

	// 별 그리기 순서로 정렬 (0→2→4→1→3→0)
	static TArray<FVector> ReorderToStarDrawOrder(const TArray<FVector>& PentagonPoints);
};
