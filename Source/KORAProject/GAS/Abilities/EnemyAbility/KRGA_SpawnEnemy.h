#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/EnemyAbility/KRGA_EnemyPlayMontage.h"
#include "KRGA_SpawnEnemy.generated.h"

UCLASS()
class KORAPROJECT_API UKRGA_SpawnEnemy : public UKRGA_EnemyPlayMontage
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = KR)
	TSubclassOf<class AKREnemyPawn> SpawnEnemyClass;
	
	UPROPERTY(EditDefaultsOnly, Category = KR)
	TObjectPtr<class UNiagaraComponent> SpawnVFX;

	UPROPERTY(EditDefaultsOnly, Category = KR)
	int32 SpawnNumber;

	UPROPERTY(EditDefaultsOnly, Category = KR)
	float SpwanStartRange;
	
	UPROPERTY(EditDefaultsOnly, Category = KR)
	float SpwanEndRange;

private:
	void SpawnEnemies();
	
	FVector GetSpawnLocation();
};
