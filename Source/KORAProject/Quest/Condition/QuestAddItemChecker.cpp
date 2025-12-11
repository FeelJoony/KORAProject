#include "QuestAddItemChecker.h"

#include "Subsystem/KRInventorySubsystem.h"

UQuestAddItemChecker::UQuestAddItemChecker()
{
	ObjectiveTag = FGameplayTag::EmptyTag;
}

bool UQuestAddItemChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	if (ObjectiveTag != InTag)
	{
		return false;
	}

	return true;
}
