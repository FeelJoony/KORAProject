#include "QuestAddItemChecker.h"

#include "Subsystem/KRInventorySubsystem.h"

UQuestAddItemChecker::UQuestAddItemChecker()
{
	Tag = FGameplayTag::EmptyTag;
}

bool UQuestAddItemChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	if (Tag != InTag)
	{
		return false;
	}

	return true;
}
