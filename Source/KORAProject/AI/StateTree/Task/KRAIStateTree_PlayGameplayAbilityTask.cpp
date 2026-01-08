#include "KRAIStateTree_PlayGameplayAbilityTask.h"

#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"
#include "AI/KREnemyPawn.h"

UKRAIStateTree_PlayGameplayAbilityTask::UKRAIStateTree_PlayGameplayAbilityTask(const FObjectInitializer& ObjectInitializer)
{
	
}

EStateTreeRunStatus UKRAIStateTree_PlayGameplayAbilityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	return Super::EnterState(Context, Transition);
}

void UKRAIStateTree_PlayGameplayAbilityTask::StateCompleted(FStateTreeExecutionContext& Context,
	const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}
