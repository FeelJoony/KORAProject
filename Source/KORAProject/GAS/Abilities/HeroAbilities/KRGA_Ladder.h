#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_Ladder.generated.h"

class AKRLadderActor;

UCLASS()
class KORAPROJECT_API UKRGA_Ladder : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_Ladder(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ladder")
	TSubclassOf<UAnimInstance> LadderAnimInstanceClass;
	
	UPROPERTY()
	TSubclassOf<UAnimInstance> SavedPrevAnimInstanceClass;
	
	UFUNCTION()
	void OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
};