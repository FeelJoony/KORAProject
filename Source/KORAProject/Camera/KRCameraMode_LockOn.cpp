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
		return;
	}

	// 현재 락온된 타겟 가져오기
	AActor* LockOnTarget = UKRGA_LockOn::GetLockedTargetFor(TargetActor);
	if (!LockOnTarget)
	{
		// 락온 타겟이 없으면 높이 오프셋을 0으로 보간
		CurrentHeightOffset = FMath::FInterpTo(CurrentHeightOffset, 0.f, DeltaTime, HeightInterpSpeed);
		return;
	}

	// 플레이어와 타겟 사이 거리 계산
	float Distance = FVector::Dist(TargetActor->GetActorLocation(), LockOnTarget->GetActorLocation());

	// 목표 높이 오프셋 계산
	float TargetHeightOffset = 0.f;

	if (DistanceToHeightCurve)
	{
		// 커브가 있으면 커브에서 값 가져오기
		TargetHeightOffset = DistanceToHeightCurve->GetFloatValue(Distance);
	}
	else
	{
		const float CloseDistance = 200.f;
		const float FarDistance = 800.f;
		const float HeightAtClose = -40.f; // 필요하다면 0.f로 설정해도 됨 (기존 150.f에서 대폭 수정)
		const float HeightAtFar = 0.f;

		TargetHeightOffset = FMath::GetMappedRangeValueClamped(
			FVector2D(CloseDistance, FarDistance),
			FVector2D(HeightAtClose, HeightAtFar),
			Distance
		);
	}

	// 높이 오프셋 범위 제한
	TargetHeightOffset = FMath::Clamp(TargetHeightOffset, 0.f, MaxHeightOffset);

	// 부드러운 보간 적용
	CurrentHeightOffset = FMath::FInterpTo(CurrentHeightOffset, TargetHeightOffset, DeltaTime, HeightInterpSpeed);

	// 카메라 위치에 높이 오프셋 적용 (시각적 표현만, ControlRotation은 건드리지 않음)
	View.Location.Z += CurrentHeightOffset;
}
