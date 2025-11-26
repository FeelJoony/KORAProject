#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "KRGA_Graple.generated.h"

class UCharacterMovementComponent;

UCLASS()
class KORAPROJECT_API UKRGA_Graple : public UKRHeroGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
private:
	void LineTrace();
	void ApplyPhysics(const FHitResult& OutHitResult, APawn* OwnerPawn);

	UFUNCTION()
	void OnAbilityEnd();

	void GoToLocation();

	void StopGraple();
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SelectedMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EndJumpMontage = nullptr;
	
	FTimerHandle GrapleTimer;
	//케이블 생성


	UPROPERTY(EditDefaultsOnly)
	float StartLaunchSpeed = 1000.f;
	
	// 이동 속도
	UPROPERTY(EditDefaultsOnly)
	float GrapleSpeed = 30000.f;

	UPROPERTY(EditDefaultsOnly)
	float StopGrapleDistance = 25000.f;
	
	UPROPERTY(EditDefaultsOnly)
	float EndDirectionZ = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	float EndLaunchSpeed = 500.f;
	
	// 목적지 (타겟 위치)
	FVector TargetLocation;

	// 현재 그라플 중인지 체크
	bool bIsGrapling = false;

	float OriginalGravity = 0.f;
};
