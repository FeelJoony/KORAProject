#include "GAS/Abilities/EnemyAbility/KRGA_SpawnEnemy.h"

#include "AI/KREnemyPawn.h"
#include "Components/StateTreeAIComponent.h"
#include "GameplayTag/KREnemyTag.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

UKRGA_SpawnEnemy::UKRGA_SpawnEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGA_SpawnEnemy::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	SpawnEnemies();
}

void UKRGA_SpawnEnemy::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_SpawnEnemy::SpawnEnemies()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	if (SpawnEnemyClass)
	{
		int32 SpawnNumber = FMath::RandRange(MinSpawnCount, MaxSpawnCount);
		for (int i = 0; i < SpawnNumber; ++i)
		{
			// FVector SpawnLocation = GetSpawnLocation();
			// if (SpawnLocation == FVector::ZeroVector)
			// {
			// 	continue;
			// }
			
			AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(
				SpawnEnemyClass, 
				CurrentActorInfo->AvatarActor->GetActorLocation(), 
				FRotator::ZeroRotator, 
				SpawnParams
			);

			if (SpawnActor)
			{
				if (AKREnemyPawn* EnemyPawn = Cast<AKREnemyPawn>(SpawnActor))
				{
					if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
					{
						UAISense_Damage::ReportDamageEvent(
							GetWorld(),
							EnemyPawn,
							PlayerCharacter,
							0,
							EnemyPawn->GetActorLocation(),
							EnemyPawn->GetActorLocation()
						);	
					}
					
					if (UStateTreeAIComponent* StateTreeAIComponent = EnemyPawn->GetComponentByClass<UStateTreeAIComponent>())
					{
						StateTreeAIComponent->SendStateTreeEvent(KRTAG_ENEMY_AISTATE_COMBAT);
					}

					SpawnActor->SetOwner(CurrentActorInfo->AvatarActor.Get());
				}
			}
		}
	}
}

FVector UKRGA_SpawnEnemy::GetSpawnLocation()
{
	if (AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get())
	{
		FVector AvatarActorLocation = AvatarActor->GetActorLocation();
		
		const float x = FMath::RandRange(0, 1) == 0 ? FMath::FRandRange(-MaxSpawnRange, -MinSpawnRange) : FMath::FRandRange(MinSpawnRange, MaxSpawnRange) + AvatarActorLocation.X; 
		const float y = FMath::RandRange(0, 1) == 0 ? FMath::FRandRange(-MaxSpawnRange, -MinSpawnRange) : FMath::FRandRange(MinSpawnRange, MaxSpawnRange) + AvatarActorLocation.Y;
		const float z = AvatarActor->GetActorLocation().Z;

		return FVector(x, y, z);
	}
	
	return FVector::ZeroVector;
}


