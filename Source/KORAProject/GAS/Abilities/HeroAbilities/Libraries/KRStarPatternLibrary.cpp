#include "KRStarPatternLibrary.h"
#include "GameFramework/Actor.h"

FKRStarPatternData UKRStarPatternLibrary::GenerateStarPattern(
	FVector Center,
	float Radius,
	FVector UpVector,
	float StartAngleOffset)
{
	FKRStarPatternData Result;
	Result.CenterLocation = Center;
	Result.Radius = Radius;

	// 1. 정오각형 꼭짓점 계산
	Result.StarPoints = CalculatePentagonPoints(Center, Radius, UpVector, StartAngleOffset);

	// 2. 별 그리기 순서로 정렬
	Result.DrawOrderPoints = ReorderToStarDrawOrder(Result.StarPoints);

	// 3. 라인 세그먼트 데이터 생성 (5개의 선분)
	Result.LineStartPoints.Empty();
	Result.LineEndPoints.Empty();

	for (int32 i = 0; i < Result.DrawOrderPoints.Num(); ++i)
	{
		int32 NextIndex = (i + 1) % Result.DrawOrderPoints.Num();
		Result.LineStartPoints.Add(Result.DrawOrderPoints[i]);
		Result.LineEndPoints.Add(Result.DrawOrderPoints[NextIndex]);
	}

	return Result;
}

FKRStarPatternData UKRStarPatternLibrary::GenerateHorizontalStarPattern(
	FVector Center,
	float Radius,
	FVector ForwardVector)
{
	FVector NormalizedForward = ForwardVector.GetSafeNormal2D();
	float StartAngle = FMath::RadiansToDegrees(FMath::Atan2(NormalizedForward.Y, NormalizedForward.X));
	
	return GenerateStarPattern(Center, Radius, FVector::UpVector, StartAngle - 90.0f);
}

FKRStarPatternData UKRStarPatternLibrary::GenerateStarPatternAroundActor(
	AActor* TargetActor,
	float Radius,
	float HeightOffset,
	bool bHorizontal)
{
	if (!TargetActor)
	{
		return FKRStarPatternData();
	}

	FVector Center = TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, HeightOffset);

	if (bHorizontal)
	{
		return GenerateHorizontalStarPattern(Center, Radius, TargetActor->GetActorForwardVector());
	}
	else
	{
		return GenerateStarPattern(Center, Radius, TargetActor->GetActorForwardVector(), 0.0f);
	}
}

FVector UKRStarPatternLibrary::GetStarPointByDrawOrder(const FKRStarPatternData& PatternData, int32 Index)
{
	if (PatternData.DrawOrderPoints.IsValidIndex(Index))
	{
		return PatternData.DrawOrderPoints[Index];
	}
	return FVector::ZeroVector;
}

FVector UKRStarPatternLibrary::GetDirectionBetweenPoints(const FKRStarPatternData& PatternData, int32 FromIndex, int32 ToIndex)
{
	if (!PatternData.DrawOrderPoints.IsValidIndex(FromIndex) ||
		!PatternData.DrawOrderPoints.IsValidIndex(ToIndex))
	{
		return FVector::ZeroVector;
	}

	return (PatternData.DrawOrderPoints[ToIndex] - PatternData.DrawOrderPoints[FromIndex]).GetSafeNormal();
}

float UKRStarPatternLibrary::GetTotalStarLineLength(const FKRStarPatternData& PatternData)
{
	float TotalLength = 0.0f;

	for (int32 i = 0; i < PatternData.DrawOrderPoints.Num(); ++i)
	{
		TotalLength += GetLineSegmentLength(PatternData, i);
	}

	return TotalLength;
}

float UKRStarPatternLibrary::GetLineSegmentLength(const FKRStarPatternData& PatternData, int32 SegmentIndex)
{
	if (!PatternData.LineStartPoints.IsValidIndex(SegmentIndex) ||
		!PatternData.LineEndPoints.IsValidIndex(SegmentIndex))
	{
		return 0.0f;
	}

	return FVector::Dist(PatternData.LineStartPoints[SegmentIndex], PatternData.LineEndPoints[SegmentIndex]);
}

TArray<FVector> UKRStarPatternLibrary::CalculatePentagonPoints(
	FVector Center,
	float Radius,
	FVector UpVector,
	float StartAngle)
{
	TArray<FVector> Points;
	Points.SetNum(5);
	
	FVector Up = UpVector.GetSafeNormal();

	// Up이 Z축에 가까우면 X축을 기준으로, 아니면 Z축을 기준으로 Right 계산
	FVector Right;
	if (FMath::Abs(FVector::DotProduct(Up, FVector::UpVector)) > 0.99f)
	{
		Right = FVector::CrossProduct(Up, FVector::ForwardVector).GetSafeNormal();
	}
	else
	{
		Right = FVector::CrossProduct(Up, FVector::UpVector).GetSafeNormal();
	}
	FVector Forward = FVector::CrossProduct(Right, Up).GetSafeNormal();

	// 정오각형: 각 꼭짓점은 72도 간격
	const float AngleStep = 72.0f;

	for (int32 i = 0; i < 5; ++i)
	{
		float AngleDegrees = StartAngle + (AngleStep * i);
		float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
		float LocalX = Radius * FMath::Cos(AngleRadians);
		float LocalY = Radius * FMath::Sin(AngleRadians);
		Points[i] = Center + (Forward * LocalX) + (Right * LocalY);
	}

	return Points;
}

TArray<FVector> UKRStarPatternLibrary::ReorderToStarDrawOrder(const TArray<FVector>& PentagonPoints)
{
	TArray<FVector> StarOrder;

	if (PentagonPoints.Num() != 5)
	{
		return StarOrder;
	}

	// 별을 그리는 순서: 0 → 2 → 4 → 1 → 3 → 0 (시작점으로 돌아와서 닫힘)
	const int32 DrawOrder[] = { 0, 2, 4, 1, 3, 0 };

	for (int32 i = 0; i < 6; ++i)
	{
		StarOrder.Add(PentagonPoints[DrawOrder[i]]);
	}

	return StarOrder;
}

FKRStarPatternData UKRStarPatternLibrary::Generate10PointStarPattern(
	FVector Center,
	float OuterRadius,
	float InnerRadius,
	FVector UpVector,
	float StartAngleOffset)
{
	FKRStarPatternData Result;
	Result.CenterLocation = Center;
	Result.Radius = OuterRadius;

	// InnerRadius가 0이면 황금비 기반 계산 (약 38.2%)
	if (InnerRadius <= 0.0f)
	{
		// 정오각별의 내부 교차점 비율: (3 - sqrt(5)) / 2 ≈ 0.382
		InnerRadius = OuterRadius * 0.382f;
	}
	
	FVector Up = UpVector.GetSafeNormal();
	FVector Right;
	if (FMath::Abs(FVector::DotProduct(Up, FVector::UpVector)) > 0.99f)
	{
		Right = FVector::CrossProduct(Up, FVector::ForwardVector).GetSafeNormal();
	}
	else
	{
		Right = FVector::CrossProduct(Up, FVector::UpVector).GetSafeNormal();
	}
	FVector Forward = FVector::CrossProduct(Right, Up).GetSafeNormal();

	// 10개의 점 생성 (외곽 5개 + 내부 5개, 교차 배치)
	// 외곽: 0, 72, 144, 216, 288도
	// 내부: 36, 108, 180, 252, 324도 (외곽 사이)
	Result.StarPoints.SetNum(10);

	for (int32 i = 0; i < 10; ++i)
	{
		float AngleDegrees = StartAngleOffset + (36.0f * i);  // 36도 간격
		float AngleRadians = FMath::DegreesToRadians(AngleDegrees);

		// 짝수 인덱스: 외곽 (0, 2, 4, 6, 8)
		// 홀수 인덱스: 내부 (1, 3, 5, 7, 9)
		float CurrentRadius = (i % 2 == 0) ? OuterRadius : InnerRadius;

		float LocalX = CurrentRadius * FMath::Cos(AngleRadians);
		float LocalY = CurrentRadius * FMath::Sin(AngleRadians);

		Result.StarPoints[i] = Center + (Forward * LocalX) + (Right * LocalY);
	}

	// DrawOrderPoints: 순서대로 연결하면 별 윤곽선 완성
	// 0(외) → 1(내) → 2(외) → 3(내) → 4(외) → 5(내) → 6(외) → 7(내) → 8(외) → 9(내) → 0(외)
	Result.DrawOrderPoints.Empty();
	for (int32 i = 0; i < 10; ++i)
	{
		Result.DrawOrderPoints.Add(Result.StarPoints[i]);
	}
	Result.DrawOrderPoints.Add(Result.StarPoints[0]);

	Result.LineStartPoints.Empty();
	Result.LineEndPoints.Empty();

	for (int32 i = 0; i < 10; ++i)
	{
		int32 NextIndex = (i + 1) % 10;
		Result.LineStartPoints.Add(Result.StarPoints[i]);
		Result.LineEndPoints.Add(Result.StarPoints[NextIndex]);
	}

	return Result;
}

FKRStarPatternData UKRStarPatternLibrary::Generate10PointStarPatternAroundActor(
	AActor* TargetActor,
	float OuterRadius,
	float HeightOffset,
	bool bHorizontal)
{
	if (!TargetActor)
	{
		return FKRStarPatternData();
	}

	FVector Center = TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, HeightOffset);

	if (bHorizontal)
	{
		FVector ForwardVec = TargetActor->GetActorForwardVector();
		FVector NormalizedForward = ForwardVec.GetSafeNormal2D();
		float StartAngle = FMath::RadiansToDegrees(FMath::Atan2(NormalizedForward.Y, NormalizedForward.X));

		return Generate10PointStarPattern(Center, OuterRadius, 0.0f, FVector::UpVector, StartAngle - 90.0f);
	}
	else
	{
		return Generate10PointStarPattern(Center, OuterRadius, 0.0f, TargetActor->GetActorForwardVector(), 0.0f);
	}
}
