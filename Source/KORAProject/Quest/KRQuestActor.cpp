#include "KRQuestActor.h"
#include "Components/StateTreeComponent.h"
#include "KRQuestInstance.h"
#include "SubSystem/KRQuestSubsystem.h"

AKRQuestActor::AKRQuestActor()
{
	QuestStateTreeComp = CreateDefaultSubobject<UStateTreeComponent>(TEXT("QuestStateTreeComp"));
}

void AKRQuestActor::SetQuestInstance(class UKRQuestInstance* NewQuestInstance)
{
	QuestInstance = NewQuestInstance;
}

UKRQuestInstance* AKRQuestActor::GetQuestInstance() const
{
	return QuestInstance;
}

