#pragma once

#include "CoreMinimal.h"
#include "KRGameplayAbility_MeleeBase.h"
#include "KRGameplayAbility_ChargeAttack.generated.h"

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_ChargeAttack : public UKRGameplayAbility_MeleeBase
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_ChargeAttack(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnKeyReleased(float TimeHeld);

	UFUNCTION()
	void OnChargeCompleted();

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

private:
	bool bIsChargeSuccess;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Charge")
	float DamageMulti_Fail;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Charge")
	float DamageMulti_Success;

	const FName SECTION_START = FName("Start");
	const FName SECTION_FAIL = FName("Attack_Fail");
	const FName SECTION_SUCCESS = FName("Attack_Success");
};
