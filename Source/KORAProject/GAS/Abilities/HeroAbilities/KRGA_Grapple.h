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
	void StartMove();

	UFUNCTION()
	void StopPlayerMove();

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector GrapPoint;
	
private:
	UFUNCTION()
	void LineTrace();

	void TargetMoveToPlayer();
	
	void CancelGrapple();
	
	UFUNCTION()
	void OnAbilityEnd();

	UFUNCTION()
	void OnLoopMontage();
		
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> StartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LoopMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> TargettGrappleMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LaunchMontage = nullptr;

	// UPROPERTY(EditDefaultsOnly)
	// TObjectPtr<UAnimMontage> CancleMontage = nullptr;
\
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> LoopMontageTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> EndMontageTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_ApplyRootMotionMoveToForce> MoveToTask=nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterial> DecalMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyImmuneGrapple;

	EGrappleState HitState = EGrappleState::Default;
	
	FTimerHandle MoveToTimer;

	FTimerHandle MaxGrappleTimer;
		
	// 에디터 설정
	UPROPERTY(EditDefaultsOnly)
	float StartLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float GrappleSpeed = 30000.f;

	UPROPERTY(EditDefaultsOnly)
	float StopGrappleDistance = 25000.f;
	
	UPROPERTY(EditDefaultsOnly)
	float EndDirectionZ = 1.f;
	
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

	//Cached
	UPROPERTY()
	TObjectPtr<ACharacter> CachedPlayerCharacter;

	UPROPERTY()
	TObjectPtr<APawn> CachedTargetPawn;

	UPROPERTY()
	TObjectPtr<UCapsuleComponent> TargetCapsuleComp=nullptr;

	uint8 bControllerRotationYaw = false;
	
	uint8 bRotationToMovement = true;
};
