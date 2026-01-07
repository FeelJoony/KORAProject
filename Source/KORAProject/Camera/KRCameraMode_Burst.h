#pragma once

#include "CoreMinimal.h"
#include "Camera/KRCameraMode_ThirdPerson.h"
#include "KRCameraMode_Burst.generated.h"

/**
 * 코어드라이브 버스트 스킬용 카메라 모드
 *
 * - 플레이어 뒤쪽 고정 위치에서 촬영
 * - 공중 난무 공격에 적합한 높이/거리 설정
 * - 타겟 없이 동작 가능 (플레이어 중심)
 * - 스킬 시작 시 카메라 위치 고정, 플레이어 추적
 */
UCLASS(Blueprintable)
class KORAPROJECT_API UKRCameraMode_Burst : public UKRCameraMode_ThirdPerson
{
	GENERATED_BODY()

public:
	UKRCameraMode_Burst();

	/**
	 * 스킬 시작 시 호출 - 카메라 고정 위치 계산
	 * @param PlayerLocation 플레이어 현재 위치
	 * @param PlayerForward 플레이어 전방 방향
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera|Burst")
	void SetupBurstCamera(const FVector& PlayerLocation, const FVector& PlayerForward);

	/** 스킬 종료 시 호출 - 상태 초기화 */
	UFUNCTION(BlueprintCallable, Category = "Camera|Burst")
	void ClearBurstCamera();

	/** 플레이어 액터 설정 (LookAt 타겟) */
	UFUNCTION(BlueprintCallable, Category = "Camera|Burst")
	void SetPlayerActor(AActor* InPlayerActor) { PlayerActor = InPlayerActor; }

	virtual void OnActivation() override;
	virtual void OnDeactivation() override;

protected:
	virtual void UpdateView(float DeltaTime) override;
	virtual FVector GetPivotLocation() const override;
	virtual FRotator GetPivotRotation() const override;

	// ─────────────────────────────────────────
	// 카메라 위치 설정
	// ─────────────────────────────────────────

	/** 플레이어 뒤쪽 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|Position")
	float BackwardDistance = 500.0f;

	/** 플레이어 위쪽 높이 오프셋 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|Position")
	float HeightOffset = 200.0f;

	/** 측면 오프셋 (우측 양수) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|Position")
	float SideOffset = 100.0f;

	/** LookAt 높이 오프셋 (플레이어 위치 기준) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|Position")
	float LookAtHeightOffset = 50.0f;

	/** 카메라 위치 보간 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|Position", meta = (ClampMin = "0.1", ClampMax = "20.0"))
	float CameraInterpSpeed = 10.0f;

	// ─────────────────────────────────────────
	// 플레이어 추적 설정
	// ─────────────────────────────────────────

	/** true: 카메라가 플레이어를 계속 바라봄, false: 카메라 회전 완전 고정 (RootMotion 스킬용) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|Tracking")
	bool bTrackPlayer = false;

	// ─────────────────────────────────────────
	// FOV 설정
	// ─────────────────────────────────────────

	/** 스킬 중 FOV */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|FOV", meta = (ClampMin = "60.0", ClampMax = "120.0"))
	float SkillFOV = 75.0f;

	/** FOV 보간 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Burst|FOV", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float FOVInterpSpeed = 5.0f;

private:
	/** 스킬 시작 시 계산된 고정 카메라 위치 */
	FVector FixedCameraLocation = FVector::ZeroVector;

	/** 스킬 시작 시 저장된 플레이어 위치 */
	FVector PlayerStartLocation = FVector::ZeroVector;

	/** 스킬 시작 시 저장된 플레이어 전방 벡터 */
	FVector PlayerStartForward = FVector::ForwardVector;

	/** 스킬 시작 시 저장된 카메라 회전 (bTrackPlayer=false일 때 사용) */
	FRotator FixedCameraRotation = FRotator::ZeroRotator;

	/** 카메라 설정 완료 여부 */
	bool bCameraSetup = false;

	/** 현재 카메라 위치 (보간용) */
	FVector CurrentCameraLocation = FVector::ZeroVector;

	/** 현재 FOV (보간용) */
	float CurrentFOV = 90.0f;

	/** 플레이어 액터 참조 */
	UPROPERTY()
	TWeakObjectPtr<AActor> PlayerActor;

	/** 타겟 카메라 위치 계산 */
	FVector CalculateTargetCameraLocation() const;

	/** LookAt 위치 계산 */
	FVector CalculateLookAtLocation() const;
};
