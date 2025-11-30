#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "KRGameplayAbility.generated.h"

class UKRCombatComponent;
class UKRAbilitySystemComponent;
class UKRCharacterMovementComponent;
class AKRBaseCharacter;
class AKRPlayerController;
class AController;

UENUM(BlueprintType)
enum class EKRAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};

UCLASS()
class KORAPROJECT_API UKRGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UKRAbilitySystemComponent;
	
public:
	UKRGameplayAbility(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "KR|Ability")
	UKRAbilitySystemComponent* GetKRAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "KR|Ability")
	AKRPlayerController* GetKRPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "KR|Ability")
	AController* GetControllerFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "KR|Ability")
	AKRBaseCharacter* GetKRCharacterFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable, Category = "KR|Ability")
	UKRCombatComponent* GetCombatComponentFromActorInfo() const;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Ability Activation")
	EKRAbilityActivationPolicy ActivationPolicy;

	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnPawnAvatarSet();

public:
	UFUNCTION(BlueprintCallable, Category = "KR|Ability")
	EKRAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
};
