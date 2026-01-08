#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Data/KRGuardTypes.h"
#include "KRGA_HeroGuard.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UKRStaminaComponent;
class AKRHeroCharacter;

/**
 * P의 거짓 스타일 가드 GA
 *
 * - 입력 직후 퍼펙트 가드 윈도우 (0.2초)
 * - 윈도우 후 일반 가드 상태 유지
 * - 스태미나 기반 가드 브레이크
 * - 모션 워핑 기반 넉백 적용
 */
UCLASS()
class KORAPROJECT_API UKRGA_HeroGuard : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_HeroGuard(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintPure, Category = "Guard")
	bool IsInPerfectGuardWindow() const { return CurrentGuardState == EGuardState::ParryWindow; }

	UFUNCTION(BlueprintPure, Category = "Guard")
	EGuardState GetCurrentGuardState() const { return CurrentGuardState; }

	void OnGuardHit(float IncomingDamage, AActor* Attacker);

	UFUNCTION(BlueprintPure, Category = "Guard")
	const FKRGuardConfig& GetGuardConfig() const { return GuardConfig; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Config")
	FKRGuardConfig GuardConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardStartMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardLoopMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardHitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> PerfectGuardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardBreakMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Cue")
	FGameplayTag PerfectGuardCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Cue")
	FGameplayTag GuardHitCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard|Cue")
	FGameplayTag GuardBreakCueTag;

private:
	void StartGuardState();
	void StopGuardState();
	void OnPerfectGuardWindowEnded();
	void TriggerGuardBreak();
	void OnGuardBreakEnded();

	void OnPerfectGuardSuccess(AActor* Attacker);
	void OnStandardGuardHit(float IncomingDamage, AActor* Attacker);

	/** 넉백에 대한 모션 워핑을 적용합니다. */
	void ApplyKnockback(AActor* Attacker, float KnockbackDistance);

	UKRStaminaComponent* GetStaminaComponent() const;
	AKRHeroCharacter* GetKRHeroCharacter() const;

	/** 현재 몽타주 태스크를 안전하게 종료합니다. */
	void StopCurrentMontageTask();

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
	EGuardState CurrentGuardState = EGuardState::None;

	FTimerHandle PerfectGuardWindowTimer;
	FTimerHandle GuardBreakTimer;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> GuardBrokenEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ParrySuccessEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> GuardHitEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};