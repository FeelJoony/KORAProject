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

protected:
	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float LockOnRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float LockOnBreakDistance = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float RotationInterpSpeed = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "KR|LockOn")
	float CameraResetInterpSpeed = 10.f;

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
	TObjectPtr<UUserWidget> LockOnWidgetInstance;

	FTimerHandle CameraResetTimerHandle;

	bool bCanSwitchTarget = true;
	
	void FindTarget();
	void ResetCamera();
	void UpdateCameraReset();

	UFUNCTION()
	void OnTick(float DeltaTime);

	void CheckForTargetSwitch();
	void SwitchTarget(FVector2D InputDirection);

	bool IsTargetValid(AActor* TargetActor) const;
	void GetAvailableTargets(TArray<AActor*>& OutActors);
	
};
