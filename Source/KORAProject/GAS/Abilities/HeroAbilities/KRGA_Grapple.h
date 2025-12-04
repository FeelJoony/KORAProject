#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_Grapple.generated.h"

class UCapsuleComponent;
class UAbilityTask_ApplyRootMotionMoveToForce;
class UAbilityTask_PlayMontageAndWait;
class UCharacterMovementComponent;

UENUM()
enum class EGrappleState : uint8
{
	Default		UMETA(DisplayName = "Default"),
	HitEnemy	UMETA(DisplayName = "Hit Enemy"),
	HitActor	UMETA(DisplayName = "Hit Actor")
};

UCLASS()
class KORAPROJECT_API UKRGA_Grapple : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Ability")
	void ApplyCableVisibility(bool Visible);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Ability")
	void ApplyCableLocation();
	
	UFUNCTION()
	void PerformLineTrace();
	UFUNCTION()
	void StartMoveLoopMontage();
	UFUNCTION()
	void OnStartMontageFinished();
	UFUNCTION()
	void StartPullLoopMontage();
	UFUNCTION()
	void OnActorGrappleFinished();
	UFUNCTION()
	void FinishAbility();
	
	void BeginStartMontage();
	
	void ProcessHitResult(const FHitResult& HitResult);
	
	/*static*/ bool IsGrappleableEnemy(APawn* Pawn);
	
	void BeginEnemyGrapple();
	
	bool ApplyStunToEnemy() const;
	
	void RemoveStunFromEnemy() const;
	
	void TickEnemyMovement();
	
	void CleanupEnemyGrapple();
	
	void BeginEndPullMontage();

	void BeginActorGrapple();
	
	void TickActorGrappleMovement();
	
	void CleanupActorGrapple();
	
	void BeginLaunchMontage();
	
	void CancelAbility();
	
	void CleanupAllTasks() const;
	
	void CleanupAllTimers();

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector GrapPoint;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> StartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LoopMoveMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LoopPullMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EndPullMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LaunchMontage = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> LoopMontageTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_ApplyRootMotionMoveToForce> MoveToTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> EndMontageTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> StartMontageTask=nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterial> DecalMaterial = nullptr;

	FTimerHandle MoveToTimer;
	FTimerHandle MaxGrappleTimer;
	
	EGrappleState HitState = EGrappleState::Default;
		
	// 에디터 설정
	UPROPERTY(EditDefaultsOnly)
	float EndLaunchSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly)
	float TraceRange = 5000.f;

	UPROPERTY(EditDefaultsOnly)
	float TargetMoveSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float MoveToSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	float EnemyGrappleDuration = 1.5f;
	
	UPROPERTY(EditDefaultsOnly)
	float StopNealyDistSquared = 200.f;
	
	UPROPERTY(EditDefaultsOnly)
	float DecalSize = 20.f;

	//Cached
	UPROPERTY()
	TObjectPtr<ACharacter> CachedPlayerCharacter;

	UPROPERTY()
	TObjectPtr<APawn> CachedTargetPawn;

	UPROPERTY()
	TObjectPtr<UCapsuleComponent> TargetCapsuleComp=nullptr;
};
