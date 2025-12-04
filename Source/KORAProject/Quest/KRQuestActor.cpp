#include "KRQuestActor.h"
#include "Components/StateTreeComponent.h"
#include "KRQuestInstance.h"
#include "SubSystem/KRQuestSubsystem.h"

AKRQuestActor::AKRQuestActor()
{
	QuestStateTreeComp = CreateDefaultSubobject<UStateTreeComponent>(TEXT("QuestStateTreeComp"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AKRQuestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	if (QuestInstance)
	{
		if (ElapsedTime >= CheckTime)
		{
			QuestInstance->AddCount(1);
			ElapsedTime = 0.f;
		}
	}
}

void AKRQuestActor::SetQuestInstance(class UKRQuestInstance* NewQuestInstance)
{
	QuestInstance = NewQuestInstance;
}

UKRQuestInstance* AKRQuestActor::GetQuestInstance() const
{
	return QuestInstance;
}

