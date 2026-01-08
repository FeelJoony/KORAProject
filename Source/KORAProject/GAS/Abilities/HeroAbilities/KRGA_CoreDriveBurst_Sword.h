#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Data/KRMeleeAttackTypes.h"
#include "KRGA_CoreDriveBurst_Sword.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UKRCoreDriveComponent;
class UGameplayEffect;
class UKRCameraMode;
class UAnimMontage;
class UKRStarDashData;

USTRUCT(BlueprintType)
struct FKRCoreDriveBurstAttackConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	EKRAttackShapeType AttackShape = EKRAttackShapeType::Sphere;

	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (EditCondition = "AttackShape == EKRAttackShapeType::Sphere", EditConditionHides))
	float SphereRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (EditCondition = "AttackShape == EKRAttackShapeType::Box", EditConditionHides))
	FVector BoxExtent = FVector(200.0f, 200.0f, 200.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float ForwardOffset = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "1", ClampMax = "20"))
	int32 HitCount = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float HitInterval = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float FinalHitDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float DamageMultiplierPerHit = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float FinalHitDamageMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<FVector> StarPatternPoints; 

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FVector FinisherLocationOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float StarCenterDistance = 300.0f;
};

UENUM(BlueprintType)
enum class EKRCoreDriveBurstPhase : uint8
{
	None,
	Startup,
	Attack,
	Finished
};

UCLASS()
class KORAPROJECT_API UKRGA_CoreDriveBurst_Sword : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_CoreDriveBurst_Sword(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	const TArray<FVector>& GetCachedWorldPoints() const { return CachedWorldPoints; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	TObjectPtr<UAnimMontage> StartupMontage;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	bool bUseRootMotion = true;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Config")
	TObjectPtr<UKRStarDashData> StarDashData;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Attack")
	FKRCoreDriveBurstAttackConfig AttackConfig;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag StartupCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag AttackCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag HitCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag FinishCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag LoopCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag BurstCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Camera")
	TSubclassOf<UKRCameraMode> AbilityCameraModeClass;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug")
	bool bShowDebugShape = false;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug", meta = (EditCondition = "bShowDebugShape"))
	float DebugDrawDuration = 0.5f;

private:
	EKRCoreDriveBurstPhase CurrentPhase = EKRCoreDriveBurstPhase::None;

	void TransitionToPhase(EKRCoreDriveBurstPhase NewPhase);
	void StartStartupPhase();
	void StartAttackPhase();

	void ApplyHyperArmor();
	void RemoveHyperArmor();
	void ApplyFullInvincibility();
	void RemoveFullInvincibility();

	void StartMultiHitSequence();
	void StopMultiHitSequence();
	void PerformMultiHit();
	void ScheduleFinalHit();
	void PerformFinalHit();

	FTimerHandle MultiHitTimerHandle;
	FTimerHandle FinalHitTimerHandle;
	int32 CurrentHitIndex = 0;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorsThisHit;

	FVector BurstAnchorLocation;
	FRotator BurstAnchorRotation;
	
	TArray<FVector> CachedWorldPoints;
	void CalculateStarPoints();

	UKRCoreDriveComponent* GetCoreDriveComponent() const;
	void ApplyDamageToTarget(AActor* TargetActor, float DamageMultiplier);
	void ExecuteCue(const FGameplayTag& CueTag);
	void ExecuteCueAtLocation(const FGameplayTag& CueTag, const FVector& Location);
	void PlayMontageWithCallback(UAnimMontage* Montage, void(UKRGA_CoreDriveBurst_Sword::*Callback)());
	
	void SetIgnorePawnCollision(bool bIgnore);
	TMap<ECollisionChannel, ECollisionResponse> PreviousCollisionResponses;
	
	void DisableMovement();
	void EnableMovement();
	void DrawDebugAttackShape();

	UFUNCTION()
	void OnStartupMontageCompleted();
	UFUNCTION()
	void OnAttackMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();

protected:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};