#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_MeleeBase.h"
#include "KRGA_EnemySlash.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class KORAPROJECT_API UKRGA_EnemySlash : public UKRGameplayAbility_MeleeBase
{
	GENERATED_BODY()

public:
	UKRGA_EnemySlash(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> SlashMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag SlashAbilityTag;

	/** 히트 체크 이벤트 태그 */
	UPROPERTY(EditDefaultsOnly, Category = "HitCheck")
	FGameplayTag HitCheckEventTag;

	/** 기본 데미지 (부모 클래스의 DamageEffectClass 사용) */
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 10.0f;

private:
	UFUNCTION()
	void OnMontageEnded();

	void SetupHitCheckEventListener();

	UFUNCTION()
	void OnHitCheckEventReceived(FGameplayEventData Payload);

	void PerformHitCheck();

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitCheckEventTask;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
};
