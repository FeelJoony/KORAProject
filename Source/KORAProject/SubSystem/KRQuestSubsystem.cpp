#include "KRQuestSubsystem.h"

#include "KRDataTablesSubsystem.h"
#include "KRInventorySubsystem.h"
#include "Data/QuestDataStruct.h"
#include "Data/SubQuestDataStruct.h"
#include "Quest/KRQuestActor.h"
#include "Quest/KRQuestDataDefinition.h"
#include "Quest/KRQuestInstance.h"
#include "Quest/Condition/QuestAddItemChecker.h"
#include "System/KRAssetManager.h"

DEFINE_LOG_CATEGORY(LogQuestSubSystem);

void UKRQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UKRDataTablesSubsystem>();
	Collection.InitializeDependency<UKRInventorySubsystem>();

	
}

void UKRQuestSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKRQuestSubsystem::AcceptQuest(int32 Index)
{
	AKRQuestActor* QuestActor = GetWorld()->SpawnActorDeferred<AKRQuestActor>(LoadQuestActorClass(Index), FTransform::Identity);

	UKRQuestInstance* QuestInstance = NewObject<UKRQuestInstance>(this);
	if (QuestInstance)
	{
		QuestInstance->Initialize(Index);
		
		ActiveInstance = QuestInstance;
		QuestActor->SetQuestInstance(QuestInstance);
	}
	
	QuestActor->FinishSpawning(FTransform::Identity);
}

bool UKRQuestSubsystem::AbandonQuest(int32 Index)
{
	return false;
}

void UKRQuestSubsystem::NotifyObjectiveEvent(FGameplayTag TargetTag, int32 Count)
{
}

TSubclassOf<AKRQuestActor> UKRQuestSubsystem::LoadQuestActorClass(int32 Index)
{
	UKRDataTablesSubsystem* DataTableSubsystem = GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	if (FQuestDataStruct* QuestData = DataTableSubsystem->GetData<FQuestDataStruct>(EGameDataType::QuestData, Index))
	{
		FPrimaryAssetId PrimaryAssetId;
		if (!PrimaryAssetId.IsValid())
		{
			PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetId(FPrimaryAssetType("KRQuestDataDefinition"), QuestData->StateTreeDefinitionPath));

			checkf(PrimaryAssetId.IsValid(), TEXT("PrimaryAssetId is invalid"));

			UKRAssetManager& AssetManager = UKRAssetManager::Get();

			TSubclassOf<UKRQuestDataDefinition> AssetClass;
			{
				FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(PrimaryAssetId);
				AssetClass = Cast<UClass>(AssetPath.TryLoad());
			}

			if (!AssetClass)
			{
				UE_LOG(LogQuestSubSystem, Error, TEXT("Failed to load QuestDataDefinition class from asset path"));
				return nullptr;
			}

			UKRQuestDataDefinition* QuestDataDefinition = AssetClass->GetDefaultObject<UKRQuestDataDefinition>();
			check(QuestDataDefinition);
			check(QuestDataDefinition->QuestActor);

			return QuestDataDefinition->QuestActor;
		}
	}

	return nullptr;
}
