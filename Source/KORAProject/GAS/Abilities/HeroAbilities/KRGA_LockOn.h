#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_LockOn.generated.h"

class UKRCameraMode;
class UUserWidget;

UCLASS()
class KORAPROJECT_API UKRGA_LockOn : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_LockOn(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	/** 현재 락온 대상 반환 */
	UFUNCTION(BlueprintPure, Category = "KR|LockOn")
	AActor* GetLockedTarget() const { return CurrentTarget; }

	/** ASC에서 활성화된 LockOn GA의 타겟을 가져오는 헬퍼 함수 */
	UFUNCTION(BlueprintPure, Category = "KR|LockOn", meta = (DefaultToSelf = "Actor"))
	static AActor* GetLockedTargetFor(AActor* Actor);
	
	// FORCEINLINE AActor* GetTarget() const
	// {
	// 	return CurrentTarget;
	// }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float LockOnRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float LockOnBreakDistance = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float RotationInterpSpeed = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	TSubclassOf<UKRCameraMode> LockOnCameraModeClass;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	TSubclassOf<UUserWidget> LockOnWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetTraceChannels;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float TargetSwitchInputThreshold = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Input")
	float TargetSwitchMouseThreshold = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Selection")
	float TargetSwitchStickThreshold = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Selection")
	float DirectionScoreWeight = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Selection")
	float DistanceScoreWeight = 0.3f;

	// ===== 피치 오프셋 설정 (거리 기반 내려다보기) =====

	/** 거리 기반 피치 조정 활성화 */
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera")
	bool bAdjustPitchBasedOnDistance = true;

	/** 피치 계산 계수 (거리 * 계수). 값이 클수록 거리 변화에 민감 */
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera", meta = (EditCondition = "bAdjustPitchBasedOnDistance"))
	float PitchDistanceCoefficient = 0.02f;

	/** 피치 기본 오프셋 (기본적으로 적용되는 내려다보기 각도) */
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera", meta = (EditCondition = "bAdjustPitchBasedOnDistance"))
	float PitchBaseOffset = 5.f;

	/** 피치 오프셋 최소값 (음수 = 아래로, 최대 내려다보기 각도) */
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera", meta = (EditCondition = "bAdjustPitchBasedOnDistance"))
	float PitchOffsetMin = -30.f;

	/** 피치 오프셋 최대값 (0 = 수평) */
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera", meta = (EditCondition = "bAdjustPitchBasedOnDistance"))
	float PitchOffsetMax = 0.f;

private:
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	FName CurrentSocketName = NAME_None;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> LockOnWidgetInstance;


	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|CameraReset")
	float CameraResetDuration = 0.2f;
	
	FRotator CameraResetStartRot;
	FRotator CameraResetTargetRot;
	float CameraResetElapsedTime = 0.f;
	
	bool bCanSwitchTarget = true;
	
	UFUNCTION()
	void OnTick(float DeltaTime);
	
	UFUNCTION()
	void ResetCamera();

	UFUNCTION()
	void OnResetCameraTick(float DeltaTime);
	
	void FindTarget();

	FVector GetTargetLocation(AActor* Target, FName SocketName) const;
	TArray<FName> GetTargetSockets(AActor* Target) const;
	
	void CheckForTargetSwitch();
	void SwitchTarget(FVector2D InputDirection);

	bool IsTargetValid(AActor* TargetActor) const;
	void GetAvailableTargets(TArray<AActor*>& OutActors);

};


