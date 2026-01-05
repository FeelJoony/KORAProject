#include "QuestAddItemChecker.h"

#include "Subsystem/KRInventorySubsystem.h"
#include "Quest/KRQuestInstance.h"

UQuestAddItemChecker::UQuestAddItemChecker()
{
	ObjectiveTag = FGameplayTag::EmptyTag;
}

UQuestConditionChecker* UQuestAddItemChecker::Initialize(UKRQuestInstance* NewQuestInstance, const FSubQuestEvalDataStruct& EvalData)
{
	ObjectiveTag = EvalData.ObjectiveTag;
	QuestInstance = NewQuestInstance;

	if (UWorld* World = NewQuestInstance->GetWorld())
	{
		ListenerHandle = UGameplayMessageSubsystem::Get(World).RegisterListener<FAddItemMessage>(
			ObjectiveTag, this, &UQuestAddItemChecker::OnAddItemMessage);
	}
	return this;
}

void UQuestAddItemChecker::Uninitialize()
{
	if (ListenerHandle.IsValid() && QuestInstance)
	{
		if (UWorld* World = QuestInstance->GetWorld())
		{
			UGameplayMessageSubsystem::Get(World).UnregisterListener(ListenerHandle);
		}
	}
}

bool UQuestAddItemChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	return EvalData.ObjectiveTag.MatchesTagExact(InTag);
}

void UQuestAddItemChecker::OnAddItemMessage(FGameplayTag Channel, const FAddItemMessage& Message)
{
	if (!ObjectiveTag.IsValid() || !QuestInstance)
	{
		return;
	}

	if (!Message.ItemTag.MatchesTagExact(ObjectiveTag))
	{
		return;
	}

	const FSubQuestEvalDataStruct& EvalData = QuestInstance->GetSubQuestEvalDataStruct();
	if (!CanCount(EvalData, Message.ItemTag))
	{
		return;
	}

	for (int32 i = 0; i < Message.StackCount; ++i)
	{
		QuestInstance->AddCount();
	}
}
