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
	CurrentProgress = (CurrentProgress + Amount) > EvalData.RequiredCount ? EvalData.RequiredCount : CurrentProgress + Amount;
}

void FSubQuestEvalData::SubtractProgress(int32 Amount)
{
	CurrentProgress = (CurrentProgress - Amount) < 0 ? 0 : CurrentProgress - Amount;
}

float FSubQuestEvalData::GetProgressRatio() const
{
	return static_cast<float>(CurrentProgress) / EvalData.RequiredCount;
}

float FSubQuestEvalData::GetRemainingTime() const
{
	return FMath::Max(0.f, EvalData.TimeLimit - ElapsedTime);
}
