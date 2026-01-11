#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_BossFight.generated.h"

class UAbilityTask_WaitGameplayEvent;

UCLASS()
class KORAPROJECT_API UKRGA_BossFight : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_BossFight(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|UI")
	FName BossNameKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Config")
	bool bEndOnDeath = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Tags")
	FGameplayTagContainer EndAbilityTags;

private:
	void ShowBossWidget();
	void HideBossWidget();
	void SendStateTreeEvent(const FGameplayTagContainer& Tags);

	UFUNCTION() void OnDeathEventReceived(FGameplayEventData Payload);

	UPROPERTY() TObjectPtr<UAbilityTask_WaitGameplayEvent> DeathEventTask;
};
