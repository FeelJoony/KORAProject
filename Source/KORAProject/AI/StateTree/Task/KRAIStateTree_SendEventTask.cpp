#include "KRAIStateTree_SendEventTask.h"
#include "AI/KRAIEnemyController.h"
#include "Components/StateTreeAIComponent.h"

EStateTreeRunStatus FKRAIStateTree_SendEventTask::EnterState(FStateTreeExecutionContext& Context,
                                                             const FStateTreeTransitionResult& Transition) const
{
	const FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);
	
	//const FInstanceData* InstanceData = Context.GetInstanceDataPtr<FInstanceData>(*this);

	if (InstanceData.EventTag.IsValid())
	{
		if (AKRAIEnemyController* KRAIController = Cast<AKRAIEnemyController>(Context.GetOwner()))
		{
			if (UStateTreeAIComponent* StateTreeComp = KRAIController->FindComponentByClass<UStateTreeAIComponent>())
			{
				FStateTreeEvent Event(InstanceData.EventTag);
				StateTreeComp->SendStateTreeEvent(Event);
			}
		}

		// Context.SendEvent(InstanceData.EventTag, FConstStructView::Make(InstanceData.Payload));
	}
	
	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FKRAIStateTree_SendEventTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return FStateTreeTaskCommonBase::Tick(Context, DeltaTime);
}

void FKRAIStateTree_SendEventTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FStateTreeTaskCommonBase::ExitState(Context, Transition);
}

void FKRAIStateTree_SendEventTask::StateCompleted(FStateTreeExecutionContext& Context,
	const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates) const
{
	FStateTreeTaskCommonBase::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

void FKRAIStateTree_SendEventTask::TriggerTransitions(FStateTreeExecutionContext& Context) const
{
	FStateTreeTaskCommonBase::TriggerTransitions(Context);

	
}
