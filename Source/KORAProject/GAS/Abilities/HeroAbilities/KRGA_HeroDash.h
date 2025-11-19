#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "KRGA_HeroDash.generated.h"

UENUM(BlueprintType)
enum class EDashDirection : uint8
{
	Forward = 0,
	ForwardRight,
	Right,
	BackwardRight,
	Backward,
	BackwardLeft,
	Left,
	ForwardLeft
};

UCLASS()
class KORAPROJECT_API UKRGA_HeroDash : public UKRHeroGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,	bool bWasCancelled) override;

	void LaunchCharacter(const FVector& InWorldDirection, ACharacter* Character);

protected:
	UFUNCTION()
	void OnAbilityEnd();
	
	EDashDirection GetDashDirection(const FVector& InputVector) const;

	UPROPERTY(EditDefaultsOnly, Category=KRSpeed)
	float DashSpeed=1500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash|Montage")
	TMap<EDashDirection, class UAnimMontage*> DashMontages;
};
