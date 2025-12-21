#include "KillNormalCreatureChecker.h"

UKillNormalCreatureChecker::UKillNormalCreatureChecker()
{
	
}

UQuestConditionChecker* UKillNormalCreatureChecker::Initialize(UKRQuestInstance* NewQuestInstance, const FSubQuestEvalDataStruct& EvalData)
{
	
	
	return this;
}

void UKillNormalCreatureChecker::Uninitialize()
{
	Super::Uninitialize();
}

bool UKillNormalCreatureChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	return Super::CanCount(EvalData, InTag);
}
