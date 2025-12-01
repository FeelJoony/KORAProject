#include "QuestStateTreeEvaluator_SubQuestProgress.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "KRQuestComponent.h"
#include "KRQuestInstance.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

void FQuestStateTreeEvaluator_SubQuestProgressInstanceData::Register(UKRQuestComponent* Component)
{
	this->QuestComponent = Component;

	UKRQuestInstance* QuestInstance = Component->GetActiveQuest();
	const FSubQuestDataStruct& SubQuestData = QuestInstance->GetSubQuestData();

	for (const FSubQuestEvalDataStruct& EvalDataStruct : SubQuestData.EvalDatas)
	{
		if (!SubQuestProgressMap.Contains(EvalDataStruct.OrderIndex))
		{
			FSubQuestEvalData EvalData;
			EvalData.Init(SubQuestData, EvalDataStruct.OrderIndex);
			
			SubQuestProgressMap.Add(EvalDataStruct.OrderIndex, EvalData);
		}
	}
}

void FQuestStateTreeEvaluator_SubQuestProgressInstanceData::NotifyProgress(FGameplayTag TargetTag, int32 Count)
{
	if (FSubQuestEvalData* EvalData = SubQuestProgressMap.Find(CurrentOrder))
	{
		if (EvalData->EvalData.ObjectiveTag != TargetTag)
		{
			return;
		}

		if (EvalData->IsCompleted())
		{
			return;
		}

		EvalData->AddProgress(Count);
	}
}

void FQuestStateTreeEvaluator_SubQuestProgressInstanceData::SetSubQuestActive(int32 SubQuestDataKey, bool bActive)
{
	SubQuestProgressMap[SubQuestDataKey].bIsActive = bActive;
}

bool FQuestStateTreeEvaluator_SubQuestProgressInstanceData::IsAllSubQuestsCompleted() const
{
	for (const auto& [Order, EvalData] : SubQuestProgressMap)
	{
		if (!EvalData.IsCompleted())
		{
			return false;
		}
	}

	return true;
}

void FQuestStateTreeEvaluator_SubQuestProgressInstanceData::AddEvalData(const FSubQuestDataStruct& SubQuestData, int32 Order)
{
	FSubQuestEvalData EvalData;
	EvalData.Init(SubQuestData, Order);
			
	SubQuestProgressMap.Add(Order, EvalData);
}

const FSubQuestEvalData* FQuestStateTreeEvaluator_SubQuestProgressInstanceData::GetEvalData(int32 Order) const
{
	return SubQuestProgressMap.Find(Order) == nullptr ? nullptr : &SubQuestProgressMap[Order];
}

void UGameplayMessageFunction::RegisterAll(UKRQuestComponent* QuestComponent)
{
	UKRQuestInstance* QuestInstance = QuestComponent->GetActiveQuest();
	const FSubQuestDataStruct& SubQuestData = QuestInstance->GetSubQuestData();
	
	for (const FSubQuestEvalDataStruct& EvalDataStruct : SubQuestData.EvalDatas)
	{
		QuestStateMessageDatas.Add(FQuestStateMessageHandleData(UGameplayMessageSubsystem::Get(this).RegisterListener(
		EvalDataStruct.ObjectiveTag,
		this,
		&ThisClass::ReceiveQuestMessageCallback)));
	}
}

void UGameplayMessageFunction::UnregisterAll()
{
	for (const FQuestStateMessageHandleData& QuestStateMessageHandleData : QuestStateMessageDatas)
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(QuestStateMessageHandleData.MessageHandle);
	}
}

void UGameplayMessageFunction::ReceiveQuestMessageCallback(FGameplayTag ObjectiveTag, const FQuestGameplayMessage& Message)
{
	InstanceData->NotifyProgress(ObjectiveTag, Message.QuestCount);
}

void FQuestStateTreeEvaluator_SubQuestProgress::TreeStart(FStateTreeExecutionContext& Context) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);
	
	UKRQuestComponent* QuestComponent = Cast<AActor>(Context.GetOwner())->GetComponentByClass<UKRQuestComponent>();
	InstanceData.Register(QuestComponent);
	MessageFunction->RegisterAll(QuestComponent);
}

void FQuestStateTreeEvaluator_SubQuestProgress::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FStateTreeEvaluatorCommonBase::Tick(Context, DeltaTime);
}

void FQuestStateTreeEvaluator_SubQuestProgress::TreeStop(FStateTreeExecutionContext& Context) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);
	UKRQuestComponent* QuestComponent = Cast<AActor>(Context.GetOwner())->GetComponentByClass<UKRQuestComponent>();
	UKRQuestInstance* QuestInstance = QuestComponent->GetActiveQuest();

	if (InstanceData.IsAllSubQuestsCompleted())
	{
		QuestInstance->CompleteQuest();
	}
	
	MessageFunction->UnregisterAll();
}
