#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "KRGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EKRAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UCLASS()
class KORAPROJECT_API UKRGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category = "KRAbility")
	EKRAbilityActivationPolicy AbilityActivationPolicy = EKRAbilityActivationPolicy::OnTriggered;
};
