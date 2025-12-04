#include "QuestAddItemChecker.h"

#include "Subsystem/KRInventorySubsystem.h"

UQuestAddItemChecker::UQuestAddItemChecker()
{
	Tag = FGameplayTag::EmptyTag;
}

bool UQuestAddItemChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag, UStruct* InStruct)
{
	if (Tag != InTag)
	{
		return false;
	}

	FAddItemMessage* AddItemMessage = reinterpret_cast<FAddItemMessage*>(InStruct);

	return true;
}
