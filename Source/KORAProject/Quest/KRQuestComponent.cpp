#include "Quest/KRQuestComponent.h"

#include "KRQuestInstance.h"
#include "GameFramework/PlayerState.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Data/QuestDataStruct.h"

DEFINE_LOG_CATEGORY(LogQuestComponent);

UKRQuestComponent::UKRQuestComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UKRQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	AcceptQuest(1);
}

void UKRQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveQuest)
	{
		ActiveQuest->TickQuest(DeltaTime);
	}

	CheckQuestCompletion();
}

bool UKRQuestComponent::AcceptQuest(int32 Index)
{
	if (CompletedQuests.Contains(Index))
	{
		return false;
	}

	APlayerState* PlayerState = Cast<APlayerState>(GetOwner());

	ActiveQuest = NewObject<UKRQuestInstance>(this);
	ActiveQuest->Initialize(Index, PlayerState);
	ActiveQuest->StartQuest();

	OnQuestAccepted.Broadcast(Index);
	UE_LOG(LogQuestComponent, Log, TEXT("Quest Accepted: %d"), Index);

	return true;
}

bool UKRQuestComponent::AbandonQuest(int32 Index)
{
	if (!ActiveQuest)
	{
		return false;
	}

	UE_LOG(LogQuestComponent, Log, TEXT("Quest Abandoned: %d"), Index);

	return true;
}

void UKRQuestComponent::NotifyObjectiveEvent(FGameplayTag TargetTag, int32 Count)
{
	
}

bool UKRQuestComponent::IsQuestCompleted(int32 Index) const
{
	return CompletedQuests.Contains(Index);
}

void UKRQuestComponent::CheckQuestCompletion()
{
	
	
}
