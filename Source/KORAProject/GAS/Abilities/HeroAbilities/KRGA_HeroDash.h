#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_HeroDash.generated.h"

UENUM(BlueprintType)
enum class EDashDirection : uint8
{
	Default = 0,
	Forward,
	ForwardRight,
	Right,
	BackwardRight,
	Backward,
	BackwardLeft,
	Left,
	ForwardLeft
};

UCLASS()
class KORAPROJECT_API UKRGA_HeroDash : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,	bool bWasCancelled) override;

	void LaunchCharacter(const FVector2D& Input, ACharacter* Character);

protected:
	UFUNCTION()
	void OnAbilityEnd();
	
	EDashDirection SelectDashDirectionByInput(const FVector2D& Input);
	FVector CalculateDirection(EDashDirection DashEnum);
	
	UPROPERTY(EditDefaultsOnly, Category=KRSpeed)
	float DashSpeed=1500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash|Montage")
	TMap<EDashDirection, class UAnimMontage*> DashMontages;
};
