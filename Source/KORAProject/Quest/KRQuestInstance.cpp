#include "Quest/KRQuestInstance.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Subsystem/KRQuestSubsystem.h"
#include "Condition/QuestConditionChecker.h"

DEFINE_LOG_CATEGORY(LogQuestInstance);

UKRQuestInstance::UKRQuestInstance()
	: CurrentState(EQuestState::NotStarted)
{
}

void UKRQuestInstance::Initialize(int32 QuestIndex)
{
	UKRDataTablesSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	CurrentQuestData = *DataTableSubsystem->GetData<FQuestDataStruct>(EGameDataType::QuestData, QuestIndex);
	CurrentSubQuestData = *DataTableSubsystem->GetData<FSubQuestDataStruct>(EGameDataType::SubQuestData, QuestIndex);
	CurrentSubOrder = CurrentSubQuestData.EvalDatas[0].OrderIndex;
	CurrentState = EQuestState::NotStarted;

	for (const FSubQuestEvalDataStruct& EvalDataStruct : CurrentSubQuestData.EvalDatas)
	{
		if (SubQuestProgressMap.Contains(EvalDataStruct.OrderIndex))
		{
			continue;
		}
		
		FSubQuestEvalData EvalData;
		EvalData.Init(CurrentSubQuestData, EvalDataStruct.OrderIndex);
		SubQuestProgressMap.Add(EvalDataStruct.OrderIndex, EvalData);
	}

	UKRQuestSubsystem::Get(GetWorld()).InitializeQuestDelegate(this);
	AddChecker();
}

void UKRQuestInstance::StartQuest()
{
	UE_LOG(LogQuestInstance, Log, TEXT("StartQuest called - Current State: %d"), (int32)CurrentState);

	if (CurrentState != EQuestState::NotStarted)
	{
		UE_LOG(LogQuestInstance, Warning, TEXT("Quest already started or completed. Current State: %d"), (int32)CurrentState);
		return;
	}

	CurrentState = EQuestState::InProgress;

	FSubQuestEvalData* EvalData = SubQuestProgressMap.Find(CurrentSubOrder);
	EvalData->Start();
	
	UE_LOG(LogQuestInstance, Log, TEXT("Quest state changed to InProgress"));

	UKRQuestSubsystem::Get(GetWorld()).OnAcceptedBroadcast(CurrentQuestData.Type, CurrentSubOrder);
}

void UKRQuestInstance::TickQuest(float DeltaTime)
{
	if (CurrentState != EQuestState::InProgress)
	{
		return;
	}
}

void UKRQuestInstance::CompleteQuest()
{
	if (CurrentState != EQuestState::InProgress)
	{
		return;
	}

	CurrentState = EQuestState::Completed;

	UKRQuestSubsystem::Get(GetWorld()).UninitializeQuestDelegate();
}

void UKRQuestInstance::FailQuest()
{
	if (CurrentState != EQuestState::InProgress)
	{
		return;
	}

	CurrentState = EQuestState::Failed;
}

void UKRQuestInstance::AddChecker()
{
	UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(GetWorld());

	for (const FSubQuestEvalDataStruct& EvalData : CurrentSubQuestData.EvalDatas)
	{
		TSubclassOf<UQuestConditionChecker> ConditionCheckerClass = QuestSubsystem.CheckerGroup.FindRef(EvalData.ObjectiveTag);
		if (ConditionCheckerClass == nullptr)
		{
			UE_LOG(LogQuestInstance, Error, TEXT("Not exist ConditionChecker! Please Check"));
			
			continue;
		}

		UQuestConditionChecker* ConditionCheckerInstance = ConditionCheckerClass->GetDefaultObject<UQuestConditionChecker>();
		QuestCheckers.Add(EvalData.ObjectiveTag, ConditionCheckerInstance);
	}
}

void UKRQuestInstance::AddCount(int32 Amount)
{
	if (CurrentState != EQuestState::InProgress)
	{
		return;
	}
	
	if (FSubQuestEvalData* EvalData = SubQuestProgressMap.Find(CurrentSubOrder))
	{
		if (EvalData->GetState() != EQuestState::InProgress || EvalData->IsCompleted())
		{
			return;
		}

		const FSubQuestEvalDataStruct& EvalDataStruct = EvalData->EvalData;
		FGameplayTag InTag = EvalDataStruct.ObjectiveTag;
		UQuestConditionChecker* ConditionCheckerInstance = QuestCheckers.FindRef(InTag);
		if (ConditionCheckerInstance == nullptr || !ConditionCheckerInstance->CanCount(EvalDataStruct, InTag))
		{
			return;
		}
		
		EvalData->AddProgress(Amount);
	}
}

void UKRQuestInstance::SetNextQuest()
{
	if (CurrentState != EQuestState::InProgress)
	{
		return;
	}

	if (FSubQuestEvalData* EvalData = SubQuestProgressMap.Find(CurrentSubOrder))
	{
		if (EvalData->IsCompleted())
		{
			EvalData->bIsActive = false;
			
			FSubQuestEvalData* NextEvalData = SubQuestProgressMap.Find(++CurrentSubOrder);
			if (CurrentSubOrder >= CurrentSubQuestData.EvalDatas.Num())
			{
				CompleteQuest();
			}
			
			NextEvalData->Start();
			NextEvalData->bIsActive = true;

			UKRQuestSubsystem::Get(GetWorld()).OnSetNextSubBroadcast(CurrentQuestData.Type, CurrentSubOrder);
		}
	}
}

