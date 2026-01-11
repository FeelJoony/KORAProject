#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "KRGA_EnemyPlayMontage.generated.h"

UCLASS()
class KORAPROJECT_API UKRGA_EnemyPlayMontage : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_EnemyPlayMontage(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
	TObjectPtr<class UAnimMontage> MontageToPlay;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
	TArray<float> PlayRates;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
	TArray<FName> SectionNames;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClassesWhileAnimating;

	UPROPERTY(EditDefaultsOnly, Category = MontageAbility)
	bool bBlockPlayOtherMontage = false;

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent);

	void GetGameplayEffectsWhileAnimating(TArray<const UGameplayEffect*>& OutEffects) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer EndAbilityTags;

	int32 CurrentSectionIndex = 0;
	int32 NumSections = 0;

	TArray<FActiveGameplayEffectHandle> AppliedEffects;

	UFUNCTION()
	void PlayNextSection();

	UFUNCTION()
	void OnSelectionCompleted();
};
