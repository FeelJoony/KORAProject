#pragma once

#include "CoreMinimal.h"
#include "KRGameplayAbility_AttackBase.h"
#include "KRGameplayAbility_MeleeBase.generated.h"

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_MeleeBase : public UKRGameplayAbility_AttackBase
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_MeleeBase(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	float CurrentDamageMultiplier = 1.f;

protected:
	void ListenForHitEvent();

	UFUNCTION()
	void OnTargetHit(FGameplayEventData Payload);
};
