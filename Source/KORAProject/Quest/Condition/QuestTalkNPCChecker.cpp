#include "QuestTalkNPCChecker.h"
#include "Quest/KRQuestInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"

UQuestTalkNPCChecker::UQuestTalkNPCChecker()
{
}

UQuestConditionChecker* UQuestTalkNPCChecker::Initialize(UKRQuestInstance* NewQuestInstance, const FSubQuestEvalDataStruct& EvalData)
{
	ObjectiveTag = EvalData.ObjectiveTag;
	QuestInstance = NewQuestInstance;

	if (UWorld* World = NewQuestInstance ? NewQuestInstance->GetWorld() : nullptr)
	{
		UGameplayMessageSubsystem& GameplayMessageSubSystem = UGameplayMessageSubsystem::Get(World);
		ListenerHandle = GameplayMessageSubSystem.RegisterListener<FQuestNPCDialogueCompleteMessage>(
			ObjectiveTag, this, &ThisClass::OnNPCDialogueComplete);
	}
	return this;
}

void UQuestTalkNPCChecker::Uninitialize()
{
	if (ListenerHandle.IsValid() && QuestInstance)
	{
		if (UWorld* World = QuestInstance->GetWorld())
		{
			UGameplayMessageSubsystem::Get(World).UnregisterListener(ListenerHandle);
		}
	}

	Super::Uninitialize();
}

bool UQuestTalkNPCChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	return Super::CanCount(EvalData, InTag);
}

void UQuestTalkNPCChecker::OnNPCDialogueComplete(FGameplayTag Channel, const FQuestNPCDialogueCompleteMessage& Payload)
{
	if (Channel == ObjectiveTag)
	{
		QuestInstance->AddCount();
	}
}
