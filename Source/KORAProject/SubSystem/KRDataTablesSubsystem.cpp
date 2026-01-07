#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Data/CitizenDataStruct.h"
#include "Data/ConsumeDataStruct.h"
#include "Data/CurrencyDataStruct.h"
#include "Data/EnemyAbilityDataStruct.h"
#include "Data/EnemyAttributeDataStruct.h"
#include "Data/EnemyDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "Data/EquipDataStruct.h"
#include "Data/ModuleDataStruct.h"
#include "Data/EquipAbilityDataStruct.h"
#include "Data/KRDataAssetTableRows.h"
#include "Data/QuestDataStruct.h"
#include "Data/ShopItemDataStruct.h"
#include "Data/SubQuestDataStruct.h"
#include "Data/TutorialDataStruct.h"
#include "Data/WeaponEnhanceDataStruct.h"

DEFINE_LOG_CATEGORY(LogDataTablesSubsystem);

UKRDataTablesSubsystem::UKRDataTablesSubsystem()
{

}

void UKRDataTablesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

<<<<<<< HEAD
	InitializeDataTables();
=======
	InitializeTableNames();

	// TableNames 맵을 순회하면서 모든 테이블 로드
	for (const auto& Pair : TableNames)
	{
		AddDataTable(Pair.Key, Pair.Value);
	}

#if !UE_BUILD_SHIPPING
	ValidateDataReferences();
#endif
>>>>>>> 99546d8a5c6df5a2d5d9a3df723f4e28ccc8df87
}

void UKRDataTablesSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKRDataTablesSubsystem::InitializeTableNames()
{
	TableNames.Empty();
	TableNames.Add(EGameDataType::ItemData, TEXT("ItemData"));
	TableNames.Add(EGameDataType::WeaponEnhanceData, TEXT("WeaponEnhanceData"));
	TableNames.Add(EGameDataType::TutorialData, TEXT("TutorialData"));
	TableNames.Add(EGameDataType::ShopItemData, TEXT("ShopItemData"));
	TableNames.Add(EGameDataType::ConsumeData, TEXT("ConsumeData"));
	TableNames.Add(EGameDataType::QuestData, TEXT("QuestData"));
	TableNames.Add(EGameDataType::SubQuestData, TEXT("SubQuestData"));
	TableNames.Add(EGameDataType::EquipData, TEXT("EquipData"));
	TableNames.Add(EGameDataType::EquipAbilityData, TEXT("EquipAbilityData"));
	TableNames.Add(EGameDataType::ModuleData, TEXT("ModuleData"));
	TableNames.Add(EGameDataType::CurrencyData, TEXT("CurrencyData"));
	TableNames.Add(EGameDataType::SoundDefinitionData, TEXT("SoundDefinitionData"));
	TableNames.Add(EGameDataType::EffectDefinitionData, TEXT("EffectDefinitionData"));
	TableNames.Add(EGameDataType::WorldEventData, TEXT("WorldEventData"));
	TableNames.Add(EGameDataType::CitizenData, TEXT("CitizenData"));
	TableNames.Add(EGameDataType::DialogueData, TEXT("DialogueData"));
	TableNames.Add(EGameDataType::LevelTransitionData, TEXT("LevelTransitionData"));
}

UKRDataTablesSubsystem& UKRDataTablesSubsystem::Get(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	check(World);
	UKRDataTablesSubsystem* DataTablesSubsystem = UGameInstance::GetSubsystem<UKRDataTablesSubsystem>(World->GetGameInstance());
	check(DataTablesSubsystem);
	return *DataTablesSubsystem;
}

UKRDataTablesSubsystem* UKRDataTablesSubsystem::GetSafe(const UObject* WorldContextObject)
{
	UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UKRDataTablesSubsystem>();
}

UCacheDataTable* UKRDataTablesSubsystem::GetTable(UScriptStruct* InStruct)
{
	TSoftObjectPtr<UCacheDataTable>& CacheDataTable = DataTables[InStruct];
	if (!CacheDataTable)
	{
		UE_LOG(LogDataTablesSubsystem, Warning, TEXT("GetTable: Table not found or invalid for type %d, reinitializing..."), (int32)InDataType);

		const FString* TableName = TableNames.Find(InDataType);
		if (TableName)
		{
			UE_LOG(LogDataTablesSubsystem, Log, TEXT("GetTable: Found TableName '%s' for type %d, calling AddDataTable..."), **TableName, (int32)InDataType);
			AddDataTable(InDataType, *TableName);
			CacheDataTablePtr = DataTables.Find(InDataType);

			if (!CacheDataTablePtr)
			{
				UE_LOG(LogDataTablesSubsystem, Error, TEXT("GetTable: AddDataTable failed - CacheDataTablePtr is still null!"));
				return nullptr;
			}
		}
		else
		{
			UE_LOG(LogDataTablesSubsystem, Error, TEXT("GetTable: Unknown data type %d - TableName not found in TableNames map"), (int32)InDataType);
			return nullptr;
		}
	}

	if (CacheDataTablePtr && CacheDataTablePtr->IsPending())
	{
		UE_LOG(LogDataTablesSubsystem, Log, TEXT("GetTable: Table is pending, loading synchronously..."));
		CacheDataTablePtr->LoadSynchronous();
	}

	UCacheDataTable* Result = CacheDataTablePtr ? CacheDataTablePtr->Get() : nullptr;
	UE_LOG(LogDataTablesSubsystem, Verbose, TEXT("GetTable returning %s for type %d"), Result ? TEXT("valid table") : TEXT("nullptr"), (int32)InDataType);
	return Result;
}

void UKRDataTablesSubsystem::ValidateDataReferences()
{
	int32 ErrorCount = 0;

	UCacheDataTable* EquipTable = GetTable(FEquipDataStruct::StaticStruct());
	UCacheDataTable* ModuleTable = GetTable(FModuleDataStruct::StaticStruct());
	UCacheDataTable* AbilityTable = GetTable(FEquipAbilityDataStruct::StaticStruct());

	if (!AbilityTable)
	{
		UE_LOG(LogDataTablesSubsystem, Error, TEXT("[Validation] EquipAbilityData table not found!"));
		return;
	}

	if (EquipTable)
	{
		UDataTable* DT = EquipTable->GetTable();
		if (!DT)
		{
			UE_LOG(LogDataTablesSubsystem, Error, TEXT("[Validation] EquipData table is null!"));
			return;
		}

		TArray<FEquipDataStruct*> EquipRows;
		DT->GetAllRows(TEXT("Validation"), EquipRows);

		for (const FEquipDataStruct* Row : EquipRows)
		{
			if (!Row) continue;

			if (Row->EquipAbilityID > 0)
			{
				const FEquipAbilityDataStruct* AbilityData = AbilityTable->FindRowSafe<FEquipAbilityDataStruct>(
						static_cast<uint32>(Row->EquipAbilityID),
						TEXT("Validation"),
						false);

				if (!AbilityData)
				{
					ErrorCount++;
				}
			}
		}
	}

	if (ModuleTable)
	{
		UDataTable* DT = ModuleTable->GetTable();
		if (!DT)
		{
			UE_LOG(LogDataTablesSubsystem, Error, TEXT("[Validation] ModuleData table is null!"));
			return;
		}

		TArray<FModuleDataStruct*> ModuleRows;
		DT->GetAllRows(TEXT("Validation"), ModuleRows);

		for (const FModuleDataStruct* Row : ModuleRows)
		{
			if (!Row) continue;

			if (Row->EquipAbilityID > 0)
			{
				const FEquipAbilityDataStruct* AbilityData = AbilityTable->FindRowSafe<FEquipAbilityDataStruct>(
						static_cast<uint32>(Row->EquipAbilityID),
						TEXT("Validation"),
						false);

				if (!AbilityData)
				{
					ErrorCount++;
				}
			}
		}
	}
	if (ErrorCount > 0)
	{
		UE_LOG(LogDataTablesSubsystem, Error, TEXT("[Validation] Found %d missing EquipAbilityID references!"), ErrorCount);
	}
	else
	{
		UE_LOG(LogDataTablesSubsystem, Log, TEXT("[Validation] All EquipAbilityID references are valid."));
	}
}

void UKRDataTablesSubsystem::InitializeDataTables()
{
	DataTables.Empty();
	
	AddDataTable<FItemDataStruct>(FString(TEXT("ItemData")));
	AddDataTable<FWeaponEnhanceDataStruct>(FString(TEXT("WeaponEnhanceData")));
	AddDataTable<FTutorialDataStruct>(FString(TEXT("TutorialData")));
	AddDataTable<FShopItemDataStruct>(FString(TEXT("ShopItemData")));
	AddDataTable<FConsumeDataStruct>(FString(TEXT("ConsumeData")));
	AddDataTable<FQuestDataStruct>(FString(TEXT("QuestData")));
	AddDataTable<FSubQuestDataStruct>(FString(TEXT("SubQuestData")));
	AddDataTable<FEquipDataStruct>(FString(TEXT("EquipData")));
	AddDataTable<FEquipAbilityDataStruct>(FString(TEXT("EquipAbilityData")));
	AddDataTable<FModuleDataStruct>(FString(TEXT("ModuleData")));
	AddDataTable<FCurrencyDataStruct>(FString(TEXT("CurrencyData")));
	AddDataTable<FSoundDefinitionDataStruct>(FString(TEXT("SoundDefinitionData")));
	AddDataTable<FEffectDefinitionDataStruct>(FString(TEXT("EffectDefinitionData")));
	AddDataTable<FWorldEventDataStruct>(FString(TEXT("WorldEventData")));
	AddDataTable<FCitizenDataStruct>(FString(TEXT("CitizenData")));
	AddDataTable<FEnemyDataStruct>(FString(TEXT("EnemyData")));
	AddDataTable<FEnemyAbilityDataStruct>(FString(TEXT("EnemyAbilityData")));
	AddDataTable<FEnemyAttributeDataStruct>(FString(TEXT("EnemyAttributeData")));

#if !UE_BUILD_SHIPPING
	ValidateDataReferences();
#endif
}

const FEquipDataStruct* UKRDataTablesSubsystem::GetEquipDataByItemTag(const FGameplayTag& ItemTag)
{
	const FItemDataStruct* ItemData = GetDataSafe<FItemDataStruct>(ItemTag);
	if (!ItemData)
	{
		return nullptr;
	}

	return GetDataSafe<FEquipDataStruct>(static_cast<uint32>(ItemData->EquipID), TEXT(""), false);
}

const FModuleDataStruct* UKRDataTablesSubsystem::GetModuleDataByItemTag(const FGameplayTag& ItemTag)
{
	const FItemDataStruct* ItemData = GetDataSafe<FItemDataStruct>(ItemTag);
	if (!ItemData)
	{
		return nullptr;
	}

	return GetDataSafe<FModuleDataStruct>(static_cast<uint32>(ItemData->ModuleID), TEXT(""), false);
}
