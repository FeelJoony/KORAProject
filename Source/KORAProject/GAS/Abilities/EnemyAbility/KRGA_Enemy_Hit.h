#pragma once

#include "CoreMinimal.h"
#include "Data/KRHitReactionTypes.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_Enemy_Hit.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class KORAPROJECT_API UKRGA_Enemy_Hit : public UKRGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;

	UFUNCTION()
	void ActivationHit();

	UFUNCTION()
	void OnMontageEnded();

	/** GameplayCue */
	//void ExecuteHitReactionCue(const FKRHitReactionCueParams& CueParams);

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY(EditDefaultsOnly, Category = KRState)
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditDefaultsOnly, Category = KRState)
	FGameplayTag HitAbilityTag;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	FGameplayTag HitReactionCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	FGameplayTag HitEffectTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	FGameplayTag HitSoundTag;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|Cue")
	EKRHitIntensity DefaultHitIntensity = EKRHitIntensity::Light;
	
	FKRHitReactionCueParams CachedCueParams;
};
