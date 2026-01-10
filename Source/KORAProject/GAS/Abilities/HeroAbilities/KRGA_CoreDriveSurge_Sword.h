#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_CoreDriveSurge_Sword.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UKRCoreDriveComponent;
class UGameplayEffect;
class UKRStarDashData;

UCLASS()
class KORAPROJECT_API UKRGA_CoreDriveSurge_Sword : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_CoreDriveSurge_Sword(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	void PerformDashDamageFromNotify();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	TObjectPtr<UAnimMontage> DashMontage;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cost")
	float CoreDriveCost = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Damage")
	float DamageMultiplier = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Data")
	TObjectPtr<UKRStarDashData> StarDashData;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Dash", meta = (ClampMin = "200.0", ClampMax = "2000.0"))
	float DashDistance = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Dash")
	FVector BoxHalfExtent = FVector(50.0f, 100.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Dash")
	FName WarpTargetName = FName("SurgeDash");

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug")
	bool bShowDebugShape = false;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug", meta = (EditCondition = "bShowDebugShape"))
	float DebugDrawDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag DashStartCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag DashHitCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Event")
	FGameplayTag HitCheckEventTag;

private:
	void SetupHitCheckEventListener();

	UFUNCTION()
	void OnHitCheckEventReceived(FGameplayEventData Payload);

	UKRCoreDriveComponent* GetCoreDriveComponent() const;
	void ApplyInvincibility();
	void RemoveInvincibility();
	void SetCollisionIgnorePawns(bool bIgnore);
	void PerformDashDamage();
	void SetupDashWarping();
	void ApplyDamageToTarget(AActor* TargetActor, const FHitResult& HitResult);
	void ExecuteDashStartCue();
	void ExecuteHitCue(const FVector& HitLocation);
	void DisableMovement();
	void EnableMovement();
	void DrawDebugHitBox();

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

protected:
	FVector DashStartLocation;
	FVector DashEndLocation;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	TEnumAsByte<ECollisionResponse> OriginalPawnResponse = ECR_Block;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitCheckEventTask;

	float OriginalGravityScale = 1.0f;
	TEnumAsByte<EMovementMode> OriginalMovementMode = MOVE_Walking;
};