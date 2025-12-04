#include "QuestStateTreeCondition_SubQuestComplete.h"

#include "KRQuestActor.h"
#include "KRQuestInstance.h"
#include "QuestStateTreeEvaluator_SubQuestProgress.h"

bool UQuestStateTreeCondition_SubQuestComplete::TestCondition(FStateTreeExecutionContext& Context) const
{
	if (AKRQuestActor* QuestActor = Cast<AKRQuestActor>(Actor))
	{
		UKRQuestInstance* QuestInstance = QuestActor->GetQuestInstance();

		return bInvert ? !QuestInstance->IsSubQuestCompleted(Order) : QuestInstance->IsSubQuestCompleted(Order);
	}

	return false;
}
