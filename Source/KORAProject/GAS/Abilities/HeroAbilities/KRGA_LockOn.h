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


