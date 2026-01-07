#include "KRAIStateTree_PlayGameplayAbilityTask.h"

#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"
#include "AI/KREnemyPawn.h"

UKRAIStateTree_PlayGameplayAbilityTask::UKRAIStateTree_PlayGameplayAbilityTask(const FObjectInitializer& ObjectInitializer)
{
	
}

EStateTreeRunStatus UKRAIStateTree_PlayGameplayAbilityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	FGameplayTagContainer AbilityTags(AbilityTag);
	if (!Actor->TryActivateAbility(AbilityTags))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to activate ability"));
		return EStateTreeRunStatus::Failed;
	}

	return Super::EnterState(Context, Transition);
}

void UKRAIStateTree_PlayGameplayAbilityTask::StateCompleted(FStateTreeExecutionContext& Context,
	const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}
