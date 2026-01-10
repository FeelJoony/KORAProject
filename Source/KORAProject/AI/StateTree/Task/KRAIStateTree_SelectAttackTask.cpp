#include "KRAIStateTree_SelectAttackTask.h"

#include "AI/KREnemyPawn.h"

UKRAIStateTree_SelectAttackTask::UKRAIStateTree_SelectAttackTask(const FObjectInitializer& ObjectInitializer)
{
	
}

EStateTreeRunStatus UKRAIStateTree_SelectAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	int32 MaxRange = 0;
	for (const auto& AbilityInfo : AbilityInfos)
	{
		if (AbilityInfo.SelectCondition.ConditionResult())
		{
			MaxRange += AbilityInfo.Weight;
		}
	}
	
	int32 CheckSelectInfoWeight = 0;
	int32 SelectInfoWeight = FMath::RandRange(0, MaxRange);
	TSubclassOf<UGameplayAbility> SelectAbilityClass = nullptr;
	float SelectAcceptableRadius = 0.0f;
	for (const auto& AbilityInfo : AbilityInfos)
	{
		if (AbilityInfo.SelectCondition.ConditionResult())
		{
			CheckSelectInfoWeight += AbilityInfo.Weight;
		}

		if (CheckSelectInfoWeight >= SelectInfoWeight)
		{
			SelectAbilityClass = AbilityInfo.AbilityClass;
			SelectAcceptableRadius = AbilityInfo.AcceptableAttackRange;

			break;
		}
	}
	
	Actor->OnSelectAttack.Broadcast(SelectAbilityClass, SelectAcceptableRadius);
	
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
