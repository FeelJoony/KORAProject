#include "KRAIStateTree_SelectAttackTask.h"

#include "AI/KREnemyPawn.h"

UKRAIStateTree_SelectAttackTask::UKRAIStateTree_SelectAttackTask(const FObjectInitializer& ObjectInitializer)
{
	
}

EStateTreeRunStatus UKRAIStateTree_SelectAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Actor->OnSelectAttack.Broadcast(AbilityClass, AcceptableAttackRange);
	
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus UKRAIStateTree_SelectAttackTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	return Super::Tick(Context, DeltaTime);
}

void UKRAIStateTree_SelectAttackTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}
