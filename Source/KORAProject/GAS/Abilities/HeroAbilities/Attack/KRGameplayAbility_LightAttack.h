#pragma once

#include "CoreMinimal.h"
#include "KRGameplayAbility_MeleeBase.h"
#include "KRGameplayAbility_LightAttack.generated.h"

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_LightAttack : public UKRGameplayAbility_MeleeBase
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_LightAttack(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

};
