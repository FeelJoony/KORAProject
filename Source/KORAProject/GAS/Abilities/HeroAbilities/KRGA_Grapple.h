#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "KRGA_Grapple.generated.h"

class UAbilityTask_ApplyRootMotionMoveToForce;
class UAbilityTask_PlayMontageAndWait;
class UCharacterMovementComponent;

UCLASS()
class KORAPROJECT_API UKRGA_Grapple : public UKRHeroGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Ability")
	void SetCableVisibility(bool Visible);

private:
	UFUNCTION()
	void LineTrace();

	void ApplyPhysics(const FHitResult& OutHitResult);

	UFUNCTION()
	void OnAbilityEnd();

	UFUNCTION()
	void OnLoopMontage();

	UFUNCTION()
	void StopGraple();
	
	UFUNCTION()
	void OnMoveToLocation();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> StartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> LoopMontage = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EndMontage = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> StartMontageTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> LoopMontageTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> EndMontageTask=nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_ApplyRootMotionMoveToForce> MoveToTask=nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterial> DecalMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EnemyImmuneGrapple;
	
	FTimerHandle RotatorTimer;
	
	//케이블 생성

	
	UPROPERTY()
	TObjectPtr<AKRHeroCharacter> CachedPlayerCharacter;

	UPROPERTY()
	TObjectPtr<AActor> CachedTargetActor;
	
	// 에디터 설정
	UPROPERTY(EditDefaultsOnly)
	float StartLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float GrapleSpeed = 30000.f;

	UPROPERTY(EditDefaultsOnly)
	float StopGrapleDistance = 25000.f;
	
	UPROPERTY(EditDefaultsOnly)
	float EndDirectionZ = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	float EndLaunchSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly)
	float RotationSpeed = 180.f;

	UPROPERTY(EditDefaultsOnly)
	float TraceRange = 5000.f;

	UPROPERTY(EditDefaultsOnly)
	float MoveToSpeed = 0.1f;
	
	// 목적지 (타겟 위치)
	FVector TargetLocation;

	FRotator TargetRotation = FRotator();

	
};
