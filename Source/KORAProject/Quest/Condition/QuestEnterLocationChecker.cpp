#include "QuestEnterLocationChecker.h"
#include "Quest/KRQuestEventTriggerBox.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Quest/KRQuestInstance.h"

UQuestEnterLocationChecker::UQuestEnterLocationChecker()
{
}

UQuestConditionChecker* UQuestEnterLocationChecker::Initialize(UKRQuestInstance* NewQuestInstance, const FSubQuestEvalDataStruct& EvalData)
{
	ObjectiveTag = EvalData.ObjectiveTag;
	QuestInstance = NewQuestInstance;

	if (UWorld* World = NewQuestInstance ? NewQuestInstance->GetWorld() : nullptr)
	{
		UGameplayMessageSubsystem& GameplayMessageSubSystem = UGameplayMessageSubsystem::Get(World);
		ListenerHandle = GameplayMessageSubSystem.RegisterListener<FQuestEventTriggerBoxMessage>(
			ObjectiveTag, this, &ThisClass::ReceiveMessage);
	}

	return this;
}

void UQuestEnterLocationChecker::Uninitialize()
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

bool UQuestEnterLocationChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	return Super::CanCount(EvalData, InTag);
}

void UQuestEnterLocationChecker::ReceiveMessage(FGameplayTag Channel, const FQuestEventTriggerBoxMessage& Payload)
{
	if (Channel == ObjectiveTag)
	{
		QuestInstance->AddCount();
	}
}
