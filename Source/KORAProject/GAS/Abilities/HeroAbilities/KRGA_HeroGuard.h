// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_HeroGuard.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class KORAPROJECT_API UKRGA_HeroGuard : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UFUNCTION()
	void OnGuardMontageLoop();

	void LockCameraRotation();
	void UnlockCameraRotation();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardStartMontage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardLoopMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Guard|Animation")
	TObjectPtr<UAnimMontage> GuardHitMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	float GuardMaxWalkSpeed = 300.0f;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> StartMontageTask = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> LoopMontageTask = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> EndMontageTask = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> HitMontageTask = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacter> CachedPlayerCharacter = nullptr;

	bool bPreviousUseControllerRotationYaw = false;
	bool bPreviousOrientRotationToMovement = false;
	float PreviousMaxWalkSpeed = 0.0f;
};