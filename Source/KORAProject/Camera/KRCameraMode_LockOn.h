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

	// ===== 거리 기반 카메라 후방 보정 (근접 시 카메라 뒤로 밀기) =====

	/** 근거리에서 카메라를 뒤로 밀지 여부 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Distance")
	bool bPushBackAtCloseRange = true;

	/** 근거리로 판단하는 기준 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Distance", meta = (EditCondition = "bPushBackAtCloseRange"))
	float CloseRangeThreshold = 350.0f;

	/** 근거리에서 최대로 뒤로 밀리는 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Distance", meta = (EditCondition = "bPushBackAtCloseRange"))
	float MaxPushBackDistance = 120.0f;

	/** 카메라 뒤로 밀기 보간 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn|Distance", meta = (EditCondition = "bPushBackAtCloseRange"))
	float PushBackInterpSpeed = 8.f;

private:
	/** 현재 적용 중인 높이 오프셋 (보간용) */
	float CurrentHeightOffset = 0.f;

	/** 현재 적용 중인 뒤로 밀기 거리 (보간용) */
	float CurrentPushBackDistance = 0.f;
};
