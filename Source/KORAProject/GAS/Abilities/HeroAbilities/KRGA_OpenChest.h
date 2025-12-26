#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_OpenChest.generated.h"

UCLASS()
class KORAPROJECT_API UKRGA_OpenChest : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_OpenChest(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Chest")
	TObjectPtr<UAnimMontage> OpenChestMontage;

	UFUNCTION()
	void OnMontageFinished();
};