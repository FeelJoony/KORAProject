#include "QuestConditionChecker.h"

UQuestConditionChecker::UQuestConditionChecker()
	: Tag(FGameplayTag::EmptyTag)
{
}

bool UQuestConditionChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	return true;
}
