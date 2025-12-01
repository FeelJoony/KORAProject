#include "QuestStateTreeTask_SubQuest.h"
#include "QuestStateTreeEvaluator_SubQuestProgress.h"
#include "Data/SubQuestDataStruct.h"
#include "SubQuestEvalData.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

DEFINE_LOG_CATEGORY(LogQuestStateTreeTask_SubQuest);

EStateTreeRunStatus FQuestStateTreeTask_SubQuest::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);

	// SubQuestTag 검증
	if (!InstanceData.SubQuestTag.IsValid())
	{
		UE_LOG(LogQuestStateTreeTask_SubQuest, Error, TEXT("[SubQuestTask] Invalid SubQuestTag"));
		DebugFailReason = TEXT("Invalid SubQuestTag");
		
		return EStateTreeRunStatus::Failed;
	}

	// DataTable에서 SubQuest 데이터 로드
	const FSubQuestDataStruct* SubQuestData = nullptr;
	if (!SubQuestData)
	{
		UE_LOG(LogQuestStateTreeTask_SubQuest, Error, TEXT("[SubQuestTask] Failed to load SubQuest data for tag: %s"), *InstanceData.SubQuestTag.ToString());
		DebugFailReason = TEXT("Failed to load SubQuest data");
		
		return EStateTreeRunStatus::Failed;
	}

	// Progress Evaluator에 SubQuest 등록
	FQuestStateTreeEvaluator_SubQuestProgressInstanceData* EvaluatorData = GetEvaluatorInstanceData(Context);
	if (!EvaluatorData)
	{
		UE_LOG(LogQuestStateTreeTask_SubQuest, Error, TEXT("[SubQuestTask] Progress Evaluator not found"));
		DebugFailReason = TEXT("Progress Evaluator not found");
		
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.bIsInitialized = true;
	EvaluatorData->CurrentOrder = InstanceData.Order;

	EvaluatorData->SetSubQuestActive(InstanceData.Order, true);

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FQuestStateTreeTask_SubQuest::Tick(FStateTreeExecutionContext& Context, float DeltaTime) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);

	if (!InstanceData.bIsInitialized)
	{
		return EStateTreeRunStatus::Failed;
	}

	// Progress Evaluator에서 진행 상태 확인
	FQuestStateTreeEvaluator_SubQuestProgressInstanceData* EvaluatorData = GetEvaluatorInstanceData(Context);
	if (!EvaluatorData)
	{
		DebugFailReason = TEXT("Progress EvaluatorInstanceData not found");
		
		return EStateTreeRunStatus::Failed;
	}

	const FSubQuestEvalData* EvalData = EvaluatorData->GetEvalData(InstanceData.Order);
	if (!EvalData)
	{
		DebugFailReason = TEXT("Progress EvalData not found");
		
		return EStateTreeRunStatus::Failed;
	}

	if (!EvalData->IsActive())
	{
		DebugFailReason = TEXT("EvalData is unactive");
		
		return EStateTreeRunStatus::Failed;
	}

	// 시간 제한 체크 (실패)
	if (EvalData->IsExistLimitTime() && EvalData->GetRemainingTime() <= 0.0f)
	{
		UE_LOG(LogQuestStateTreeTask_SubQuest, Warning, TEXT("[SubQuestTask] SubQuest failed (timeout): %s"), *InstanceData.SubQuestTag.ToString());
		DebugFailReason = TEXT("Timeout");
		
		return EStateTreeRunStatus::Failed;
	}
	
	// 완료 체크
	if (EvalData->IsCompleted())
	{
		UE_LOG(LogQuestStateTreeTask_SubQuest, Log, TEXT("[SubQuestTask] SubQuest completed: %s"), *InstanceData.SubQuestTag.ToString());
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FQuestStateTreeTask_SubQuest::StateCompleted(FStateTreeExecutionContext& Context,
	const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates) const
{
	FStateTreeTaskCommonBase::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

void FQuestStateTreeTask_SubQuest::ExitState(FStateTreeExecutionContext& Context,
                                             const FStateTreeTransitionResult& Transition) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);

	if (!InstanceData.bIsInitialized)
	{
		return;
	}

	if (Transition.CurrentRunStatus == EStateTreeRunStatus::Failed)
	{
		UE_LOG(LogQuestStateTreeTask_SubQuest, Error, TEXT("Fail Reason: %s"), *DebugFailReason);
		
		return;
	}

	// SubQuest 비활성화
	FQuestStateTreeEvaluator_SubQuestProgressInstanceData* EvaluatorData = GetEvaluatorInstanceData(Context);
	if (EvaluatorData)
	{
		EvaluatorData->SetSubQuestActive(InstanceData.Order, false);
	}

	InstanceData.bIsInitialized = false;
	UE_LOG(LogQuestStateTreeTask_SubQuest, Log, TEXT("[SubQuestTask] SubQuest deactivated: %s"), *InstanceData.SubQuestTag.ToString());
}

bool FQuestStateTreeTask_SubQuest::Link(FStateTreeLinker& Linker)
{
	// Evaluator InstanceData 링크 (자동으로 매칭됨)
	Linker.LinkExternalData(ProgressEvaluatorHandle);

	return FStateTreeTaskCommonBase::Link(Linker);
}

FQuestStateTreeEvaluator_SubQuestProgressInstanceData* FQuestStateTreeTask_SubQuest::GetEvaluatorInstanceData(FStateTreeExecutionContext& Context) const
{
	// Handle을 통해 Evaluator InstanceData 접근
	return Context.GetExternalDataPtr(ProgressEvaluatorHandle);
}
