#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_HitReaction.generated.h"

UENUM(BlueprintType)
enum class EKRHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

UCLASS()
class KORAPROJECT_API UKRGA_HitReaction : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_HitReaction(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<UAnimMontage> MontageFront;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<UAnimMontage> MontageBack;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<UAnimMontage> MontageLeft;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<UAnimMontage> MontageRight;


protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

private:
	EKRHitDirection CalculateHitDirection(const FGameplayEventData* TriggerEventData);
	UAnimMontage* GetMontageByDirection(EKRHitDirection Direction) const;
};
