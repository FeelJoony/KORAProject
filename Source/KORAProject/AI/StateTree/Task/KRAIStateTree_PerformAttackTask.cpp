#include "KRAIStateTree_PerformAttackTask.h"

#include "AbilitySystemComponent.h"
#include "AI/KREnemyPawn.h"
#include "Abilities/GameplayAbility.h"

EStateTreeRunStatus UKRAIStateTree_PerformAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (Actor && AbilityClass != nullptr)
	{
		if (!Actor->TryActivateAbility(AbilityClass))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to activate ability"));

			return EStateTreeRunStatus::Failed;
		}
	}
	
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus UKRAIStateTree_PerformAttackTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	return Super::Tick(Context, DeltaTime);
}

void UKRAIStateTree_PerformAttackTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	Actor->OnUnselectAttack.Broadcast();
	
	Super::ExitState(Context, Transition);
}
