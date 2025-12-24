#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_AttackBase.h"
#include "Data/KRMeleeAttackTypes.h"
#include "KRGameplayAbility_MeleeAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
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
	// 외부 호출용 (AnimNotify에서 호출)
	// ─────────────────────────────────────────────────────

	/** 히트 체크 시작 (AnimNotifyState_MeleeHitCheck에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "KR|MeleeAttack")
	void BeginHitCheck();

	/** 히트 체크 수행 (AnimNotifyState_MeleeHitCheck의 Tick에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "KR|MeleeAttack")
	void PerformHitCheck();

	/** 히트 체크 종료 (AnimNotifyState_MeleeHitCheck에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "KR|MeleeAttack")
	void EndHitCheck();

	/** 현재 공격 설정 반환 (AnimNotify에서 참조) */
	UFUNCTION(BlueprintPure, Category = "KR|MeleeAttack")
	const FKRMeleeAttackConfig& GetCurrentAttackConfig() const;

protected:
	// ─────────────────────────────────────────────────────
	// 공격 설정
	// ─────────────────────────────────────────────────────

	/** 콤보별 공격 설정 배열 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Config")
	TArray<FKRMeleeAttackConfig> ComboAttackConfigs;

	/** 기본 공격 설정 (콤보 설정이 없을 때 사용) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Config")
	FKRMeleeAttackConfig DefaultAttackConfig;

	// ─────────────────────────────────────────────────────
	// GameplayCue 태그
	// ─────────────────────────────────────────────────────

	/** 휘두르기 이펙트/사운드 큐 태그 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Cue")
	FGameplayTag SwingCueTag;

	/** 타격 이펙트/사운드 큐 태그 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Cue")
	FGameplayTag HitCueTag;

	// ─────────────────────────────────────────────────────
	// 모션 워핑
	// ─────────────────────────────────────────────────────

	/** 모션 워핑 사용 여부 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|MotionWarping")
	bool bUseMotionWarping = true;

	/** 모션 워핑 타겟 이름 (AnimNotifyState_MotionWarping과 일치해야 함) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|MotionWarping")
	FName WarpTargetName = FName("AttackWarp");

	/** 적과 최소 거리 유지 (겹침 방지) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|MotionWarping")
	float MinApproachDistance = 100.0f;

	/** 타겟 탐지 범위 (모션 워핑용) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|MotionWarping")
	float TargetSearchRadius = 500.0f;

	// ─────────────────────────────────────────────────────
	// 데미지
	// ─────────────────────────────────────────────────────

	/** 데미지 GameplayEffect 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// ─────────────────────────────────────────────────────
	// 디버그
	// ─────────────────────────────────────────────────────

	/** 히트 체크 디버그 시각화 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Debug")
	bool bDrawDebugHitCheck = false;

protected:
	// ─────────────────────────────────────────────────────
	// 파생 클래스에서 구현할 함수
	// ─────────────────────────────────────────────────────

	/** 현재 콤보에 맞는 몽타주 배열 반환 (파생 클래스에서 오버라이드) */
	virtual const TArray<TObjectPtr<UAnimMontage>>* GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const;

	// ─────────────────────────────────────────────────────
	// 내부 함수
	// ─────────────────────────────────────────────────────

	/** EquipmentManager 조회 */
	UKREquipmentManagerComponent* GetEquipmentManager() const;

	/** MotionWarpingComponent 조회 */
	UMotionWarpingComponent* GetMotionWarpingComponent() const;

	/** 현재 콤보에 맞는 몽타주 조회 */
	UAnimMontage* GetCurrentMontage() const;

	/** 모션 워핑 설정 */
	void SetupMotionWarping();

	/** 타겟 탐지 (락온 또는 최근접 적) */
	AActor* FindBestTarget(const FKRMeleeAttackConfig& Config) const;

	/** Shape Trace 실행 */
	bool PerformShapeTrace(TArray<FHitResult>& OutHitResults) const;

	/** Cone 형태 Trace */
	bool PerformConeTrace(TArray<FHitResult>& OutHitResults) const;

	/** Box 형태 Trace */
	bool PerformBoxTrace(TArray<FHitResult>& OutHitResults) const;

	/** Sphere 형태 Trace */
	bool PerformSphereTrace(TArray<FHitResult>& OutHitResults) const;

	/** Capsule 형태 Trace */
	bool PerformCapsuleTrace(TArray<FHitResult>& OutHitResults) const;

	/** 히트 결과 처리 */
	void ProcessHitResults(const TArray<FHitResult>& HitResults);

	/** 단일 대상 히트 처리 */
	void ApplyHitToTarget(AActor* HitActor, const FHitResult& HitResult);

	/** 데미지 적용 */
	void ApplyDamage(AActor* TargetActor, const FHitResult& HitResult);

	/** HitReaction 이벤트 전송 */
	void SendHitReactionEvent(AActor* TargetActor, const FHitResult& HitResult);

	/** 휘두르기 GameplayCue 실행 (몽타주 시작 시) */
	void ExecuteSwingCue();

	/** 히트 GameplayCue 실행 */
	void ExecuteHitCue(const FHitResult& HitResult);

	/** 히트스톱 적용 */
	void ApplyHitStop();

	/** 카메라 쉐이크 적용 */
	void ApplyCameraShake();

	/** 몽타주 콜백 */
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

protected:
	// ─────────────────────────────────────────────────────
	// 런타임 상태
	// ─────────────────────────────────────────────────────

	/** 현재 공격에서 이미 히트한 대상들 (중복 히트 방지) */
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorsThisSwing;

	/** 현재 히트 체크 중인지 */
	bool bIsHitCheckActive = false;

	/** 몽타주 태스크 */
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
