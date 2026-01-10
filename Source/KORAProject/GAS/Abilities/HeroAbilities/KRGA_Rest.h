#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_Rest.generated.h"

class UAbilityTask_PlayMontageAndWait;
class AKRCheckpointActor;
class AKRWorldSpawner;

/**
 * 휴식 GA (체크포인트에서 사용)
 *
 * 기능:
 * - 앉기 시작 몽타주 재생
 * - 체크포인트 저장 및 Save UI 표시
 * - 휴식 대기 시간 동안 앉기 루프 몽타주 재생
 * - 모든 적 재스폰
 * - 일어나기 몽타주 재생 후 종료
 */
UCLASS()
class KORAPROJECT_API UKRGA_Rest : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_Rest(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** 앉기 시작 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest|Animation")
	TObjectPtr<UAnimMontage> SitDownStartMontage;

	/** 앉기 루프 몽타주 (휴식 대기 중 재생) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest|Animation")
	TObjectPtr<UAnimMontage> SitLoopMontage;

	/** 일어나기 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest|Animation")
	TObjectPtr<UAnimMontage> StandUpMontage;

	/** 휴식 대기 시간 (앉기 루프 재생 시간) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest|Config")
	float RestDuration = 1.5f;

private:
	UFUNCTION()
	void OnSitDownStartCompleted();

	UFUNCTION()
	void OnSitDownStartInterrupted();

	UFUNCTION()
	void OnSitLoopCompleted();

	UFUNCTION()
	void OnSitLoopInterrupted();

	UFUNCTION()
	void OnStandUpCompleted();

	UFUNCTION()
	void OnStandUpInterrupted();

	/** 앉기 루프 몽타주 재생 시작 */
	void PlaySitLoopMontage();

	/** 휴식 타이머 만료 시 일어나기 시작 */
	void OnRestTimerExpired();

	/** 체크포인트 저장 및 Save UI 표시 */
	void SaveCheckpointAndShowUI();

	/** 모든 적 재스폰 */
	void RespawnAllEnemies();

	/** 현재 상호작용 중인 체크포인트 액터 찾기 */
	AKRCheckpointActor* FindCurrentCheckpoint() const;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	FTimerHandle RestTimerHandle;
};
