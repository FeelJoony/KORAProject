#pragma once

#include "CoreMinimal.h"
#include "KRCameraMode_ThirdPerson.h"
#include "Curves/CurveFloat.h"
#include "KRCameraMode_LockOn.generated.h"

/**
 * 락온 시 사용되는 카메라 모드
 * - GA(KRGA_LockOn)에서 ControlRotation을 제어
 * - CameraMode에서는 카메라 위치(높이 오프셋)만 조정
 */
UCLASS(Blueprintable)
class KORAPROJECT_API UKRCameraMode_LockOn : public UKRCameraMode_ThirdPerson
{
	GENERATED_BODY()

public:
	UKRCameraMode_LockOn();

protected:
	virtual void UpdateView(float DeltaTime) override;

	// ===== 거리 기반 카메라 높이 조정 (시각적 표현만) =====

	/** 타겟까지 거리에 따른 추가 Z 오프셋 커브 (가까울수록 카메라를 높이 올림) */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Height")
	TObjectPtr<const UCurveFloat> DistanceToHeightCurve;

	/** 커브가 없을 때 사용할 기본 높이 오프셋 계수 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Height")
	float DefaultHeightCoefficient = 0.1f;

	/** 최대 높이 오프셋 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Height")
	float MaxHeightOffset = 150.f;

	/** 높이 오프셋 보간 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Height")
	float HeightInterpSpeed = 5.f;

private:
	/** 현재 적용 중인 높이 오프셋 (보간용) */
	float CurrentHeightOffset = 0.f;
};
