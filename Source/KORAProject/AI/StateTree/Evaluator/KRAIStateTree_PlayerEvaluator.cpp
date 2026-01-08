#include "KRAIStateTree_PlayerEvaluator.h"
#include "AI/KREnemyPawn.h"
#include "Characters/KRHeroCharacter.h"
#include "Kismet/GameplayStatics.h"

void UKRAIStateTree_PlayerEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	// Fallback: EQS가 설정하지 않으면 직접 찾기
	if (!PlayerActor)
	{
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
		{
			PlayerActor = PlayerPawn;
			UE_LOG(LogTemp, Warning, TEXT("[%s] PlayerEvaluator TreeStart - Found Player via GetPlayerPawn: %s"), *GetNameSafe(Actor), *GetNameSafe(PlayerActor));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] PlayerEvaluator TreeStart - Failed to find Player!"), *GetNameSafe(Actor));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] PlayerEvaluator TreeStart - PlayerActor already set (via EQS): %s"), *GetNameSafe(Actor), *GetNameSafe(PlayerActor));
	}

	SetDefaultValues();
}

void UKRAIStateTree_PlayerEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (!PlayerActor)
	{
		SetDefaultValues();
		return;
	}

	DistanceToPlayer = FVector::Dist(Actor->GetActorLocation(), PlayerActor->GetActorLocation());
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
