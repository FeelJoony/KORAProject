#pragma once

#include "CoreMinimal.h"
#include "Camera/KRCameraMode_ThirdPerson.h"
#include "KRCameraMode_StarDash.generated.h"

/**
 * StarDash 스킬용 카메라 모드
 *
 * 타겟이 화면 중앙에 오도록 고정된 위치에서
 * 스킬을 사용하는 플레이어를 따라 바라봅니다.
 * 카메라 위치는 스킬 시작 시 고정되고, 시선만 플레이어를 추적
 */
UCLASS(Blueprintable)
class KORAPROJECT_API UKRCameraMode_StarDash : public UKRCameraMode_ThirdPerson
{
	GENERATED_BODY()

public:
	UKRCameraMode_StarDash();
	/**
	 * 스킬 시작 위치 설정 (카메라 고정 위치 계산)
	 * @param TargetLocation 타겟(적) 위치 - 화면 중앙에 배치
	 * @param PlayerStartLocation 플레이어 시작 위치 - 카메라 방향 기준
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera|StarDash")
	void SetSkillLocations(FVector TargetLocation, FVector PlayerStartLocation);


	UFUNCTION(BlueprintCallable, Category = "Camera|StarDash")
	void ClearSkillLocations();
	UFUNCTION(BlueprintCallable, Category = "Camera|StarDash")
	void SetPlayerActor(AActor* InPlayerActor) { PlayerActor = InPlayerActor; }
	
	virtual void OnActivation() override;
	virtual void OnDeactivation() override;

protected:
	virtual void UpdateView(float DeltaTime) override;
	virtual FVector GetPivotLocation() const override;
	virtual FRotator GetPivotRotation() const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|Position")
	float HeightOffset = 400.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|Position")
	float BackwardDistance = 600.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|Position")
	float LookAtHeightOffset = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|Position")
	bool bFixedCameraPosition = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|Position", meta = (ClampMin = "0.1", ClampMax = "20.0"))
	float CameraInterpSpeed = 8.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|Position")
	bool bUsePlayerBackward = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|FOV", meta = (ClampMin = "60.0", ClampMax = "120.0"))
	float SkillFOV = 90.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StarDash|FOV", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float FOVInterpSpeed = 5.0f;

private:
	FVector TargetLocation = FVector::ZeroVector; // 화면 중앙 (타겟 위치) 
	FVector PlayerStartLocation = FVector::ZeroVector;
	FVector FixedCameraLocation = FVector::ZeroVector;
	
	bool bSkillLocationSet = false;
	
	FVector CurrentCameraLocation = FVector::ZeroVector;
	float CurrentFOV = 90.0f;
	UPROPERTY() TWeakObjectPtr<AActor> PlayerActor;
	
	FVector CalculateTargetCameraLocation() const;
	FVector CalculateLookAtLocation() const;
};
