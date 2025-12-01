#include "QuestStateTreeCondition_SubQuestComplete.h"
#include "QuestStateTreeEvaluator_SubQuestProgress.h"
#include "SubQuestEvalData.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

FQuestStateTreeCondition_SubQuestComplete::FQuestStateTreeCondition_SubQuestComplete()
{
}

bool FQuestStateTreeCondition_SubQuestComplete::Link(FStateTreeLinker& Linker)
{
	// Evaluator InstanceData 링크 (자동으로 매칭됨)
	Linker.LinkExternalData(ProgressEvaluatorHandle);

	return FStateTreeConditionCommonBase::Link(Linker);
}

bool FQuestStateTreeCondition_SubQuestComplete::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);

	const FQuestStateTreeEvaluator_SubQuestProgressInstanceData* EvaluatorData = GetEvaluatorInstanceData(Context);
	if (!EvaluatorData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SubQuestComplete] Evaluator not found"));
		return false;
	}

	const FSubQuestEvalData* EvalData = EvaluatorData->GetEvalData(InstanceData.Order);
	if (!EvalData)
	{
		return false;
	}

	const bool bIsCompleted = EvalData->IsCompleted();
	return bIsCompleted;
}

const FQuestStateTreeEvaluator_SubQuestProgressInstanceData* FQuestStateTreeCondition_SubQuestComplete::
GetEvaluatorInstanceData(FStateTreeExecutionContext& Context) const
{
	// Handle을 통해 Evaluator InstanceData 접근
	return Context.GetExternalDataPtr(ProgressEvaluatorHandle);
}
