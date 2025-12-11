#include "QuestConditionChecker.h"

UQuestConditionChecker::UQuestConditionChecker()
	: ObjectiveTag(FGameplayTag::EmptyTag)
{
}

bool UQuestConditionChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	return true;
}
