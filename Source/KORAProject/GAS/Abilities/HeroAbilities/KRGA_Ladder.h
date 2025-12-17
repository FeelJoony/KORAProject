#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "KRGA_Ladder.generated.h"

class AKRLadderActor;
class UKRCameraMode;
class UAnimMontage;

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
	TSubclassOf<UKRCameraMode> LadderCameraMode;

	UPROPERTY(EditDefaultsOnly, Category = "Ladder")
	TObjectPtr<UAnimMontage> MountTopMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ladder")
	TObjectPtr<UAnimMontage> DismountTopMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ladder")
	TObjectPtr<UAnimMontage> DismountBottomMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Ladder")
	TSubclassOf<UAnimInstance> LadderAnimInstanceClass;

	UPROPERTY(EditDefaultsOnly, Category = "Ladder")
	float LadderTopHandOffset = 60.0f;
	
	UPROPERTY()
	TSubclassOf<UAnimInstance> SavedPrevAnimInstanceClass;

	UFUNCTION()
	void OnReachedTop(FGameplayEventData Payload);

	UFUNCTION()
	void OnReachedBottom(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
};