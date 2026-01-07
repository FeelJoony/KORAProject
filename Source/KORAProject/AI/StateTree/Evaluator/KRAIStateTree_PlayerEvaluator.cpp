#include "KRAIStateTree_PlayerEvaluator.h"
#include "AI/KREnemyPawn.h"
#include "Characters/KRHeroCharacter.h"
#include "Kismet/GameplayStatics.h"

void UKRAIStateTree_PlayerEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
}

void UKRAIStateTree_PlayerEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (!PlayerActor)
	{
		SetDefaultValues();
		
		return;
	}

	DistanceToPlayer = FVector::DistSquared(Actor->GetActorLocation(), PlayerActor->GetActorLocation());
	if (AKRHeroCharacter* PlayerCharacter = Cast<AKRHeroCharacter>(PlayerActor))
	{
		
	}
}

void UKRAIStateTree_PlayerEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}

void UKRAIStateTree_PlayerEvaluator::SetDefaultValues()
{
	DistanceToPlayer = 0.f;
	bIsPlayerAttacking = false;
	bIsPlayerDead = false;
}
