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

	UGameplayMessageSubsystem& GameplayMessageSubSystem = UGameplayMessageSubsystem::Get(GetWorld());
	GameplayMessageSubSystem.RegisterListener(ObjectiveTag, this, &ThisClass::ReceiveMessage);

	return this;
}

void UQuestEnterLocationChecker::Uninitialize()
{
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
