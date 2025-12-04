#include "Quest/SubQuestEvalData.h"

void FSubQuestEvalData::Init(const FSubQuestDataStruct& SubQuestData, int32 Order)
{
	EvalData = SubQuestData.GetEvalData(Order);

	CurrentProgress = 0;
	ElapsedTime = 0.f;
	bIsActive = true;
	bFailQuestOnFailure = false;
}

void FSubQuestEvalData::AddProgress(int32 Amount)
{
	bool bIsCompleted = (CurrentProgress + Amount) >= EvalData.RequiredCount;
	if (bIsCompleted)
	{
		CurrentProgress = EvalData.RequiredCount;
		Complete();

		return;
	}
	
	CurrentProgress += Amount;
	
}

void FSubQuestEvalData::SubtractProgress(int32 Amount)
{
	bool bIsZero = CurrentProgress - Amount <= 0;
	if (bIsZero)
	{
		CurrentProgress = 0;
	}
	
	CurrentProgress -= Amount;
}

float FSubQuestEvalData::GetProgressRatio() const
{
	return (static_cast<float>(CurrentProgress) / EvalData.RequiredCount) * 100.f;
}

float FSubQuestEvalData::GetRemainingTime() const
{
	return FMath::Max(0.f, EvalData.TimeLimit - ElapsedTime);
}

void FSubQuestEvalData::Start()
{
	CurrentState = EQuestState::InProgress;
}

void FSubQuestEvalData::Complete()
{
	CurrentState = EQuestState::Completed;
}

void FSubQuestEvalData::Fail()
{
	CurrentState = EQuestState::Failed;
}


