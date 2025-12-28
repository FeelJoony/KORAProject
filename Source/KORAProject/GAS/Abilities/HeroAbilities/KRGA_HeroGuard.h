#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Data/KRGuardTypes.h"
#include "KRGA_HeroGuard.generated.h"

class UAbilityTask_WaitInputRelease;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UKRStaminaComponent;

/**
 * P의 거짓 스타일 가드 GA
 *
 * - 입력 직후 퍼펙트 가드 윈도우 (0.2초)
 * - 윈도우 후 일반 가드 상태 유지
 * - 스태미나 기반 가드 브레이크
 */
UCLASS()
class KORAPROJECT_API UKRGA_HeroGuard : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_HeroGuard(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// ─────────────────────────────────────────
	// 외부 호출용 (데미지 시스템에서 호출)
	// ─────────────────────────────────────────

	/** 현재 퍼펙트 가드 상태인지 확인 */
	UFUNCTION(BlueprintPure, Category = "Guard")
	bool IsInPerfectGuardWindow() const { return CurrentGuardState == EGuardState::ParryWindow; }

	/** 현재 가드 상태 조회 */
	UFUNCTION(BlueprintPure, Category = "Guard")
	EGuardState GetCurrentGuardState() const { return CurrentGuardState; }

	/** 가드 히트 시 스태미나 소모 (GEExecCalc에서 호출) */
	void OnGuardHit(float IncomingDamage, AActor* Attacker);

protected:
	// ─────────────────────────────────────────
	// 설정
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Config")
	FKRGuardConfig GuardConfig;

	// ─────────────────────────────────────────
	// 애니메이션
	// ─────────────────────────────────────────

	/** 가드 시작 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardStartMontage;

	/** 가드 유지 몽타주 (루프) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardLoopMontage;

	/** 가드 피격 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardHitMontage;

	/** 퍼펙트 가드 성공 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> PerfectGuardMontage;

	/** 가드 브레이크 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardBreakMontage;

	// ─────────────────────────────────────────
	// GameplayCue
	// ─────────────────────────────────────────

	/** 퍼펙트 가드 이펙트/사운드 큐 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Cue")
	FGameplayTag PerfectGuardCueTag;

	/** 일반 가드 히트 큐 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Cue")
	FGameplayTag GuardHitCueTag;

	/** 가드 브레이크 큐 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Cue")
	FGameplayTag GuardBreakCueTag;

	// ─────────────────────────────────────────
	// 내부 함수
	// ─────────────────────────────────────────

	/** 가드 상태 시작 */
	void StartGuardState();

	/** 가드 상태 종료 */
	void StopGuardState();

	/** 퍼펙트 가드 윈도우 종료 콜백 */
	void OnPerfectGuardWindowEnded();

	/** 가드 브레이크 처리 */
	void TriggerGuardBreak();

	/** 가드 브레이크 종료 콜백 */
	void OnGuardBreakEnded();

	/** 퍼펙트 가드 성공 처리 */
	void OnPerfectGuardSuccess(AActor* Attacker);

	/** 일반 가드 히트 처리 */
	void OnStandardGuardHit(float IncomingDamage, AActor* Attacker);

	/** 스태미나 컴포넌트 조회 */
	UKRStaminaComponent* GetStaminaComponent() const;

	// ─────────────────────────────────────────
	// 이벤트 핸들러
	// ─────────────────────────────────────────

	UFUNCTION()
	void OnGuardInputReleased(float TimeHeld);

	UFUNCTION()
	void OnGuardBrokenEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnParrySuccessEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnGuardHitEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

protected:
	// ─────────────────────────────────────────
	// 런타임 상태
	// ─────────────────────────────────────────

	/** 현재 가드 상태 */
	EGuardState CurrentGuardState = EGuardState::None;

	/** 퍼펙트 가드 윈도우 타이머 */
	FTimerHandle PerfectGuardWindowTimer;

	/** 가드 브레이크 타이머 */
	FTimerHandle GuardBreakTimer;

	/** 태스크들 */
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> InputReleaseTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> GuardBrokenEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ParrySuccessEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> GuardHitEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
