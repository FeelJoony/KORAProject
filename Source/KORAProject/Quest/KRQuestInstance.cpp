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
	UKRDataTablesSubsystem& DataTableSubsystem = UKRDataTablesSubsystem::Get(this);
	CurrentQuestData = *DataTableSubsystem.GetData<FQuestDataStruct>(QuestIndex);
	CurrentSubQuestData = *DataTableSubsystem.GetData<FSubQuestDataStruct>(QuestIndex);
	CurrentSubOrder = CurrentSubQuestData.EvalDatas[0].OrderIndex;
	CurrentState = EQuestState::NotStarted;

	UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(GetWorld());
	for (const FSubQuestEvalDataStruct& EvalDataStruct : CurrentSubQuestData.EvalDatas)
	{
		if (SubQuestProgressMap.Contains(EvalDataStruct.OrderIndex))
		{
			continue;
		}
		
		FSubQuestEvalData EvalData;
		EvalData.Init(CurrentSubQuestData, EvalDataStruct.OrderIndex);
		SubQuestProgressMap.Add(EvalDataStruct.OrderIndex, EvalData);

		// Initialize Checkers

		UQuestConditionChecker* Checker = QuestSubsystem.CheckerGroup.FindRef(EvalDataStruct.ObjectiveTag)->GetDefaultObject<UQuestConditionChecker>()->Initialize(this, EvalDataStruct);
		if (Checker)
		{
			QuestCheckers.Add(EvalDataStruct.ObjectiveTag, Checker);
		}
	}

	UKRQuestSubsystem::Get(GetWorld()).InitializeQuestDelegate(this);
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

	for (const auto& CheckerKeyValue : QuestCheckers)
	{
		CheckerKeyValue.Value->Uninitialize();
	}

	QuestCheckers.Empty();
	
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

