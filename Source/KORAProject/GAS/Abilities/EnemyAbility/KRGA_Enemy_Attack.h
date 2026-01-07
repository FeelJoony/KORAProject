#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Data/KRMeleeAttackTypes.h"
#include "KRGA_Enemy_Attack.generated.h"

/**
 * Enemy 공격 GameplayAbility
 * AnimNotify_EnemyMeleeHitCheck에서 PerformHitCheck()를 호출하여 히트 체크 수행
 */
UCLASS()
class KORAPROJECT_API UKRGA_Enemy_Attack : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_Enemy_Attack(const FObjectInitializer& ObjectInitializer);

	// AnimNotify에서 호출
	UFUNCTION(BlueprintCallable, Category = "KR|EnemyAttack")
	void PerformHitCheck();

	UFUNCTION(BlueprintPure, Category = "KR|EnemyAttack")
	const FKRMeleeAttackConfig& GetCurrentAttackConfig() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Config")
	FKRMeleeAttackConfig AttackConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Cue")
	FGameplayTag SwingCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Cue")
	FGameplayTag HitCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|Debug")
	bool bDrawDebugHitCheck = false;

protected:
	bool PerformShapeTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformConeTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformBoxTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformSphereTrace(TArray<FHitResult>& OutHitResults) const;
	bool PerformCapsuleTrace(TArray<FHitResult>& OutHitResults) const;

	void ProcessHitResults(const TArray<FHitResult>& HitResults);
	void ApplyHitToTarget(AActor* HitActor, const FHitResult& HitResult);
	void ApplyDamage(AActor* TargetActor, const FHitResult& HitResult);
	void SendHitReactionEvent(AActor* TargetActor, const FHitResult& HitResult);
	void ExecuteSwingCue();
	void ExecuteHitCue(const FHitResult& HitResult);
	void ApplyCameraShake();

protected:
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorsThisSwing;
};
