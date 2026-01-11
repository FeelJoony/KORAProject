#pragma once

#include "CoreMinimal.h"
#include "KRGA_EnemyPlayMontage.h"
#include "KRGA_SpawnEnemy.generated.h"

UCLASS()
class KORAPROJECT_API UKRGA_SpawnEnemy : public UKRGA_EnemyPlayMontage
{
	GENERATED_BODY()

public:
	UKRGA_SpawnEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = KR)
	TSubclassOf<class AKREnemyPawn> SpawnEnemyClass;
	
	UPROPERTY(EditDefaultsOnly, Category = KR)
	TObjectPtr<class UNiagaraComponent> SpawnVFX;

	UPROPERTY(EditDefaultsOnly, Category = KR)
	float MinSpawnRange;
	
	UPROPERTY(EditDefaultsOnly, Category = KR)
	float MaxSpawnRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = KR)
	int32 MinSpawnCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = KR)
	int32 MaxSpawnCount = 0;

private:
	void SpawnEnemies();
	
	FVector GetSpawnLocation();
};
