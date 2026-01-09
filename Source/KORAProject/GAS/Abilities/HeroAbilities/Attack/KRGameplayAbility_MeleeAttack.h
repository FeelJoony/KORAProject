#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_AttackBase.h"
#include "Data/KRMeleeAttackTypes.h"
#include "KRGameplayAbility_MeleeAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UMotionWarpingComponent;
class UKREquipmentManagerComponent;
struct FKRAppliedEquipmentEntry;

/**
 * 근접 공격 Base GA
 *
 * 주요 기능:
 * - EquipmentManager에서 몽타주 조회
 * - AnimNotifyState 기반 히트 감지 트리거
 * - Shape Trace로 대상 탐지
 * - GameplayCue로 이펙트/사운드 재생
 * - Motion Warping으로 이동 제어
 */
UCLASS(Abstract)
class KORAPROJECT_API UKRGameplayAbility_MeleeAttack : public UKRGameplayAbility_AttackBase
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_MeleeAttack(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// ─────────────────────────────────────────────────────
	// 히트 체크 (GameplayEvent로 트리거됨)
	// ─────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "KR|MeleeAttack")
	void BeginHitCheck();

	UFUNCTION(BlueprintCallable, Category = "KR|MeleeAttack")
	void PerformHitCheck();

	UFUNCTION(BlueprintCallable, Category = "KR|MeleeAttack")
	void EndHitCheck();
	
	UFUNCTION(BlueprintPure, Category = "KR|MeleeAttack")
	const FKRMeleeAttackConfig& GetCurrentAttackConfig() const;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
	float PlayRates = 1.f;

protected:
	// ─────────────────────────────────────────────────────
	// 공격 설정
	// ─────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Config")
	TArray<FKRMeleeAttackConfig> ComboAttackConfigs;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Config")
	FKRMeleeAttackConfig DefaultAttackConfig;

	// ─────────────────────────────────────────────────────
	// GameplayCue 태그
	// ─────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Cue")
	FGameplayTag SwingCueTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Cue")
	FGameplayTag HitCueTag;

	// ─────────────────────────────────────────────────────
	// 모션 워핑
	// ─────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|MotionWarping")
	bool bUseMotionWarping = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|MotionWarping")
	FName WarpTargetName = FName("AttackWarp");

	// ─────────────────────────────────────────────────────
	// 데미지
	// ─────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// ─────────────────────────────────────────────────────
	// 스태미나
	// ─────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Stamina")
	float AttackStaminaCost = 12.0f;

	// ─────────────────────────────────────────────────────
	// 디버그
	// ─────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Debug")
	bool bDrawDebugHitCheck = false;

protected:
	// ─────────────────────────────────────────────────────
	// 파생 클래스에서 구현할 함수
	// ─────────────────────────────────────────────────────
	virtual const TArray<TObjectPtr<UAnimMontage>>* GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const;

	// ─────────────────────────────────────────────────────
	// 내부 함수
	// ─────────────────────────────────────────────────────
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	// ─────────────────────────────────────────────────────
	// GameplayEvent 리스너
	// ─────────────────────────────────────────────────────
	void SetupHitCheckEventListeners();

	UFUNCTION()
	void OnHitCheckBeginEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnHitCheckTickEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnHitCheckEndEvent(FGameplayEventData Payload);
	
	UKREquipmentManagerComponent* GetEquipmentManager() const;
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
	virtual UAnimMontage* GetCurrentMontage() const;
	void SetupMotionWarping();
	AActor* FindBestTarget() const;
	bool PerformShapeTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformConeTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformBoxTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformSphereTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformCapsuleTrace(TArray<FHitResult>& OutHitResults) const;
	void ProcessHitResults(const TArray<FHitResult>& HitResults);
	void ApplyHitToTarget(AActor* HitActor, const FHitResult& HitResult);
	void ApplyDamage(AActor* TargetActor, const FHitResult& HitResult);
	void SendHitReactionEvent(AActor* TargetActor, const FHitResult& HitResult);
	void ExecuteSwingCue();
	void ExecuteHitCue(const FHitResult& HitResult);
	void ApplyHitStop();
	void ApplyCameraShake();

protected:
	// ─────────────────────────────────────────────────────
	// 런타임 상태
	// ─────────────────────────────────────────────────────
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorsThisSwing;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitCheckBeginTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitCheckTickTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitCheckEndTask;

	bool bIsHitCheckActive = false;
	int32 ActiveComboIndex = 0;
	bool BeginOnlyTriggerOnce = true;
};
