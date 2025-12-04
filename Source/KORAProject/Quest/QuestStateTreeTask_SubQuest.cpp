#include "QuestStateTreeTask_SubQuest.h"

#include "KRQuestActor.h"
#include "KRQuestInstance.h"
#include "QuestStateTreeEvaluator_SubQuestProgress.h"
#include "Data/SubQuestDataStruct.h"
#include "StateTreeExecutionContext.h"

DEFINE_LOG_CATEGORY(LogQuestStateTreeTask_SubQuest);

EStateTreeRunStatus FQuestStateTreeTask_SubQuest::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);

	AKRQuestActor* QuestActor = CastChecked<AKRQuestActor>(Context.GetOwner());
	if (UKRQuestInstance* QuestInstance = QuestActor->GetQuestInstance())
	{
		QuestInstance->SetNextQuest();

		UE_LOG(LogQuestStateTreeTask_SubQuest, Log, TEXT("[SubQuestTask] SubQuest activated: %s"), *QuestInstance->GetSubQuestEvalDataStruct().SubQuestName.ToString());
	}
	
	InstanceData.bIsInitialized = true;

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FQuestStateTreeTask_SubQuest::Tick(FStateTreeExecutionContext& Context, float DeltaTime) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);
	if (!InstanceData.bIsInitialized)
	{
		return EStateTreeRunStatus::Failed;
	}

	AKRQuestActor* QuestActor = CastChecked<AKRQuestActor>(Context.GetOwner());
	if (UKRQuestInstance* QuestInstance = QuestActor->GetQuestInstance())
	{
		if (QuestInstance->IsSubQuestCompleted(InstanceData.Order))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	return EStateTreeRunStatus::Running;
}

void FQuestStateTreeTask_SubQuest::ExitState(FStateTreeExecutionContext& Context,
                                             const FStateTreeTransitionResult& Transition) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);
	if (!InstanceData.bIsInitialized)
	{
		return;
	}

	InstanceData.bIsInitialized = false;

	AKRQuestActor* QuestActor = CastChecked<AKRQuestActor>(Context.GetOwner());
	if (UKRQuestInstance* QuestInstance = QuestActor->GetQuestInstance())
	{
		UE_LOG(LogQuestStateTreeTask_SubQuest, Log, TEXT("[SubQuestTask] SubQuest deactivated: %s"), *QuestInstance->GetSubQuestEvalDataStruct().SubQuestName.ToString());
	}
}

