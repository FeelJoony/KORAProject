#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_MeleeAttack.h"
#include "KRGameplayAbility_EnemyAttack.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_EnemyAttack : public UKRGameplayAbility_MeleeAttack
{
	GENERATED_BODY()
public:
	//UKRGameplayAbility_EnemyAttack(const FObjectInitializer& ObjectInitializer);
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> MontageToPlay;
	
	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClassesWhileAnimating;
protected:
	virtual UAnimMontage* GetCurrentMontage() const override;
	virtual void ProcessHitResults(const TArray<FHitResult>& HitResults) override;
	
	void SetupEndSectionEventListener();
	
	UFUNCTION()
	void OnEndSectionReached(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer EndAbilityTags;

	TArray<FActiveGameplayEffectHandle> AppliedEffects;

	UPROPERTY() TObjectPtr<UAbilityTask_WaitGameplayEvent> EndSectionTask;
};
