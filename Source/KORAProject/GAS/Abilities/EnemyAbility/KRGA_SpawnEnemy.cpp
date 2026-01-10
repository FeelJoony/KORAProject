#include "GAS/Abilities/EnemyAbility/KRGA_SpawnEnemy.h"

#include "AI/KREnemyPawn.h"

void UKRGA_SpawnEnemy::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UKRGA_SpawnEnemy::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_SpawnEnemy::SpawnEnemies()
{
	FVector SpawnLocation = GetSpawnLocation();
	if (SpawnLocation == FVector::ZeroVector) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	if (SpawnEnemyClass)
	{
		for (int i=0; i<SpawnNumber; ++i)
		{
			GetWorld()->SpawnActor<AActor>(
				SpawnEnemyClass, 
				SpawnLocation, 
				FRotator::ZeroRotator, 
				SpawnParams
			);
		}
	}
}

FVector UKRGA_SpawnEnemy::GetSpawnLocation()
{
	
	
	
	return FVector::ZeroVector;
}
