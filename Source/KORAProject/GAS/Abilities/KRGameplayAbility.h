#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "KRGameplayAbility.generated.h"

class UPawnCombatComponent;
class UKRAbilitySystemComponent;

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

	UPROPERTY(EditDefaultsOnly, Category = "KR|Ability")
	EKRAbilityActivationPolicy AbilityActivationPolicy = EKRAbilityActivationPolicy::OnTriggered;

	UFUNCTION(BlueprintPure, Category = "KR|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "KR|Ability")
	UKRAbilitySystemComponent* GetKRAbilitySystemComponentFromActorInfo() const;
};
