#pragma once

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "KRAIStateTree_SendEventTask.generated.h"

USTRUCT(BlueprintType)
struct FSendEventTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Context", meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(BlueprintReadOnly, Category = "Context", meta = (Context))
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(EditAnywhere, Category = "Send Event")
	FGameplayTag EventTag;
};

USTRUCT(Category = "KRAI", meta = (DisplayName = "Send State Tree Event", ToolTip = "A task that sends an event to the StateTree.", Keywords = "Event, Task, AI, KRAI"))
struct KORAPROJECT_API FKRAIStateTree_SendEventTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceData = FSendEventTaskInstanceData;
	
public:
	FKRAIStateTree_SendEventTask() = default;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceData::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates) const override;

	virtual void TriggerTransitions(FStateTreeExecutionContext& Context) const override;
};
