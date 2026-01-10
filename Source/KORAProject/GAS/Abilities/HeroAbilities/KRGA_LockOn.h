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

	UFUNCTION(BlueprintPure, Category = "KR|LockOn")
	AActor* GetLockedTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintPure, Category = "KR|LockOn", meta = (DefaultToSelf = "Actor"))
	static AActor* GetLockedTargetFor(AActor* Actor);

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

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Input")
	float TargetSwitchMouseThreshold = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Selection")
	float TargetSwitchStickThreshold = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Selection")
	float DirectionScoreWeight = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Selection")
	float DistanceScoreWeight = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera")
	bool bAdjustPitchBasedOnDistance = true;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera", meta = (EditCondition = "bAdjustPitchBasedOnDistance"))
	float PitchDistanceCoefficient = 0.02f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera", meta = (EditCondition = "bAdjustPitchBasedOnDistance"))
	float PitchBaseOffset = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn|Camera", meta = (EditCondition = "bAdjustPitchBasedOnDistance"))
	float PitchOffsetMin = -30.f;

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

	/** 락온 타겟을 향해 캐릭터의 ControlRotation을 업데이트합니다. */
	void UpdateTargetRotation(float DeltaTime);
	
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