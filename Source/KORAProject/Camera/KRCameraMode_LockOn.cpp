#include "KRCameraMode_LockOn.h"
#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"

UKRCameraMode_LockOn::UKRCameraMode_LockOn()
{
	// 시야각 조정
	FieldOfView = 85.f;

	// 락온 시 피치 범위 확장 (더 아래를 볼 수 있도록)
	ViewPitchMin = -60.f;
	ViewPitchMax = 30.f;
}

void UKRCameraMode_LockOn::UpdateView(float DeltaTime)
{
	// 부모 클래스의 기본 뷰 업데이트 먼저 수행
	Super::UpdateView(DeltaTime);

	// 락온 타겟이 있는지 확인
	AActor* TargetActor = GetTargetActor();
	if (!TargetActor)
	{
		CurrentHeightOffset = FMath::FInterpTo(CurrentHeightOffset, 0.f, DeltaTime, HeightInterpSpeed);
		CurrentPushBackDistance = FMath::FInterpTo(CurrentPushBackDistance, 0.f, DeltaTime, PushBackInterpSpeed);
		return;
	}

	// 현재 락온된 타겟 가져오기
	AActor* LockOnTarget = UKRGA_LockOn::GetLockedTargetFor(TargetActor);
	if (!LockOnTarget)
	{
		// 락온 타겟이 없으면 오프셋을 0으로 보간
		CurrentHeightOffset = FMath::FInterpTo(CurrentHeightOffset, 0.f, DeltaTime, HeightInterpSpeed);
		CurrentPushBackDistance = FMath::FInterpTo(CurrentPushBackDistance, 0.f, DeltaTime, PushBackInterpSpeed);
		return;
	}

	// 플레이어와 타겟 사이 거리 계산
	const float Distance = FVector::Dist(TargetActor->GetActorLocation(), LockOnTarget->GetActorLocation());

	// ===== 높이 오프셋 계산 =====
	float TargetHeightOffset = 0.f;

	if (DistanceToHeightCurve)
	{
		TargetHeightOffset = DistanceToHeightCurve->GetFloatValue(Distance);
	}
	else
	{
		const float CloseDistance = 200.f;
		const float FarDistance = 800.f;
		const float HeightAtClose = -40.f;
		const float HeightAtFar = 0.f;

		TargetHeightOffset = FMath::GetMappedRangeValueClamped(
			FVector2D(CloseDistance, FarDistance),
			FVector2D(HeightAtClose, HeightAtFar),
			Distance
		);
	}

	TargetHeightOffset = FMath::Clamp(TargetHeightOffset, 0.f, MaxHeightOffset);
	CurrentHeightOffset = FMath::FInterpTo(CurrentHeightOffset, TargetHeightOffset, DeltaTime, HeightInterpSpeed);

	// ===== 거리 기반 카메라 뒤로 밀기 =====
	float TargetPushBackDistance = 0.f;

	if (bPushBackAtCloseRange && Distance < CloseRangeThreshold)
	{
		// 가까울수록 카메라를 뒤로 더 밀기
		const float CloseRatio = 1.0f - (Distance / CloseRangeThreshold);
		TargetPushBackDistance = CloseRatio * MaxPushBackDistance;
	}

	CurrentPushBackDistance = FMath::FInterpTo(CurrentPushBackDistance, TargetPushBackDistance, DeltaTime, PushBackInterpSpeed);

	// ===== 최종 카메라 위치 적용 =====
	// 높이 오프셋
	View.Location.Z += CurrentHeightOffset;

	// 뒤로 밀기 (카메라 회전 기준 후방으로)
	if (CurrentPushBackDistance > KINDA_SMALL_NUMBER)
	{
		FVector BackDirection = -View.Rotation.Vector();
		BackDirection.Z = 0.f;
		BackDirection.Normalize();
		View.Location += BackDirection * CurrentPushBackDistance;
	}
}
