#include "Quest/KRQuestInstance.h"

#include "KRQuestDataDefinition.h"
#include "StateTreeSchema.h"
#include "Components/StateTreeComponent.h"
#include "GameFramework/PlayerState.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "System/KRAssetManager.h"
#include "StateTree.h"

DEFINE_LOG_CATEGORY(LogQuestInstance);

UKRQuestInstance::UKRQuestInstance()
	: CurrentState(EQuestState::NotStarted)
{
}

void UKRQuestInstance::Initialize(int32 QuestIndex, APlayerState* InOwningPlayer)
{
	if (!StateTreeComponent)
	{
		UStateTreeComponent* NewStateTreeComp = InOwningPlayer->GetComponentByClass<UStateTreeComponent>();
		check(NewStateTreeComp);

		StateTreeComponent = NewStateTreeComp;
	}
	
	UKRDataTablesSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	QuestData = *DataTableSubsystem->GetData<FQuestDataStruct>(EGameDataType::QuestData, QuestIndex);
	SubQuestData = *DataTableSubsystem->GetData<FSubQuestDataStruct>(EGameDataType::SubQuestData, QuestIndex);
	CurrentSubOrder = SubQuestData.EvalDatas[0].OrderIndex;
	OwningPlayer = InOwningPlayer;
	CurrentState = EQuestState::NotStarted;
	
	FPrimaryAssetId PrimaryAssetId;
	if (!PrimaryAssetId.IsValid())
	{
		PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType("KRQuestStateTreeSchemaData"), QuestData.StateTreeDefinitionName);

		SetStateTreeAsset(PrimaryAssetId);
	}

	if (StateTreeAsset)
	{
		StateTreeComponent->SetStateTree(StateTreeAsset);
	}
}

void UKRQuestInstance::SetStateTreeAsset(FPrimaryAssetId PrimaryAssetId)
{
	if (!PrimaryAssetId.IsValid())
	{
		return;
	}

	UKRAssetManager& AssetManager = UKRAssetManager::Get();

	TSubclassOf<UKRQuestDataDefinition> AssetClass;
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(PrimaryAssetId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad());
	}

	UKRQuestDataDefinition* QuestDataDefinition = AssetClass->GetDefaultObject<UKRQuestDataDefinition>();
	check(QuestDataDefinition);
	check(QuestDataDefinition->QuestStateTree)

	StateTreeAsset = QuestDataDefinition->QuestStateTree;

	check(StateTreeComponent);

	StateTreeComponent->SetStateTree(StateTreeAsset);
}

void UKRQuestInstance::StartQuest()
{
	if (CurrentState != EQuestState::NotStarted)
	{
		return;
	}

	CurrentState = EQuestState::InProgress;

	if (StateTreeComponent)
	{
		//StateTreeComponent->RegisterComponent();
		//StateTreeComponent->Activate();

		StateTreeComponent->Activate();
		StateTreeComponent->StartLogic();
	}

	//UE_LOG(LogQuestInstance, Log, TEXT("Quest %s started"), *QuestTag.ToString());
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

	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic(TEXT("Success Quest"));
		StateTreeComponent->Deactivate();
	}

	//UE_LOG(LogQuestInstance, Log, TEXT("Quest %s completed"), *QuestTag.ToString());
}

void UKRQuestInstance::FailQuest()
{
	if (CurrentState != EQuestState::InProgress)
	{
		return;
	}

	CurrentState = EQuestState::Failed;

	if (StateTreeComponent)
	{
		StateTreeComponent->Deactivate();
	}

	//UE_LOG(LogQuestInstance, Log, TEXT("Quest %s failed"), *QuestTag.ToString())
}

