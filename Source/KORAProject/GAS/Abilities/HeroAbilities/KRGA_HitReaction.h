#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Data/KRHitReactionTypes.h"
#include "KRGA_HitReaction.generated.h"

class UMotionWarpingComponent;

/**
 * HitReaction GA
 * - 플레이어/적 모두 사용 가능
 * - 방향별 몽타주 + GameplayCue로 이펙트/사운드 처리 -> 공격마다 커스터마이징 가능 
 * - 넉백/Motion Warping/히트스톱 지원
 */
UCLASS()
class KORAPROJECT_API UKRGA_HitReaction : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_HitReaction(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Animation")
	TObjectPtr<UAnimMontage> MontageFront;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Animation")
	TObjectPtr<UAnimMontage> MontageBack;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Animation")
	TObjectPtr<UAnimMontage> MontageLeft;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Animation")
	TObjectPtr<UAnimMontage> MontageRight;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	FGameplayTag HitReactionCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	FGameplayTag HitEffectTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	FGameplayTag HitSoundTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	FName EffectSocketName = NAME_None;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Knockback")
	bool bUseKnockback = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Knockback", meta = (EditCondition = "bUseKnockback"))
	float KnockbackDistance = 100.0f;


	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|MotionWarping")
	bool bUseMotionWarping = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|MotionWarping", meta = (EditCondition = "bUseMotionWarping"))
	FName WarpTargetName = "KnockbackTarget";

	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|HitStop")
	bool bUseHitStop = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|HitStop", meta = (EditCondition = "bUseHitStop"))
	float HitStopDuration = 0.1f;

	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Intensity")
	EKRHitIntensity DefaultHitIntensity = EKRHitIntensity::Light; // Effect, Sound BY Intensity in Cue

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

private:
	UAnimMontage* GetMontageByDirection(EKRHitDirection Direction) const;

	void ExecuteHitReactionCue(const FKRHitReactionCueParams& CueParams);
	void ApplyKnockback(const FVector& HitDirection); // Motion Warping 
	void ApplyHitStop();
	
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
	FKRHitReactionCueParams CachedCueParams;
};
