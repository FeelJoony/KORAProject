#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_EnemyAttack.h"
#include "KRGA_EnemyBackstepAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UKRAbilityTask_WaitTick;

UCLASS()
class KORAPROJECT_API UKRGA_EnemyBackstepAttack : public UKRGameplayAbility_EnemyAttack
{
	GENERATED_BODY()

public:
	UKRGA_EnemyBackstepAttack(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Backstep")
	TObjectPtr<UAnimMontage> BackstepMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Backstep")
	float BackstepDistance = 700.f;

	UPROPERTY(EditDefaultsOnly, Category = "Backstep")
	float BackstepSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Backstep")
	bool bFacePlayerOnBackstep = true;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackMontagePlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Recovery")
	float RecoveryMontagePlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "HitCheck")
	float HitCheckSphereRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "HitCheck")
	float HitCheckForwardOffset = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 10.0f;

	virtual void PerformHitCheck() override;

private:
	void StartBackstepPhase();
	void StartAttackPhase();
	void StartRecoveryPhase();

	UFUNCTION()
	void OnBackstepTick(float DeltaTime);
	UFUNCTION()
	void OnBackstepMontageCompleted();
	UFUNCTION()
	void OnBackstepMontageInterrupted();
	UFUNCTION()
	void OnAttackMontageCompleted();
	UFUNCTION()
	void OnAttackMontageInterrupted();
	UFUNCTION()
	void OnRecoveryMontageCompleted();
	UFUNCTION()
	void OnRecoveryMontageInterrupted();

	enum class EAttackPhase : uint8
	{
		None,
		Backstep,
		Attack,
		Recovery
	};
	EAttackPhase CurrentPhase = EAttackPhase::None;

	UPROPERTY()
	TObjectPtr<UKRAbilityTask_WaitTick> BackstepTickTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> BackstepMontageTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> AttackMontageTask;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> RecoveryMontageTask;

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedPlayerActor;

	FVector BackstepTargetLocation;
	FRotator BackstepTargetRotation;
};
