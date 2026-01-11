#include "QuestDelegate.h"
#include "Data/SubQuestDataStruct.h"

void UQuestDelegate::Initialize(class UKRQuestInstance* NewQuestInstance)
{
	if (QuestInstance != nullptr)
	{
		return;
	}
	
	QuestInstance = NewQuestInstance;
}

void UQuestDelegate::Reinitialize(class UKRQuestInstance* NewQuestInstance)
{
	Uninitialize();
	Initialize(NewQuestInstance);
}

void UQuestDelegate::Uninitialize()
{
	OnQuestAccepted.Clear();
	OnQuestCompleted.Clear();
	OnQuestFailed.Clear();
	OnQuestSetNextSub.Clear();
	OnQuestProgress.Clear();
	
	QuestInstance = nullptr;
}

void UQuestDelegate::OnAcceptedBroadcast(int32 OrderIndex)
{
	if (OnQuestAccepted.IsBound())
	{
		OnQuestAccepted.Broadcast(OrderIndex);
	}
}

void UQuestDelegate::OnCompletedBroadcast(int32 OrderIndex)
{
	if (OnQuestCompleted.IsBound())
	{
		OnQuestCompleted.Broadcast(OrderIndex);
	}
}

void UQuestDelegate::OnFailedBroadcast(int32 OrderIndex)
{
	if (OnQuestFailed.IsBound())
	{
		OnQuestFailed.Broadcast(OrderIndex);
	}
}

void UQuestDelegate::OnSetNextSubBroadcast(int32 OrderIndex)
{
	if (OnQuestSetNextSub.IsBound())
	{
		OnQuestSetNextSub.Broadcast(OrderIndex);
	}
}

void UQuestDelegate::OnProgressBroadcast(int32 OrderIndex, const FSubQuestEvalData& EvalData)
{
	if (OnQuestProgress.IsBound())
	{
		OnQuestProgress.Broadcast(OrderIndex, EvalData);
	}
}
