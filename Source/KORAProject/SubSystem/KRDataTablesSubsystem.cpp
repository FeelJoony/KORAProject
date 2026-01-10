#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Data/CitizenDataStruct.h"
#include "Data/DialogueDataStruct.h"
#include "Data/ConsumeDataStruct.h"
#include "Data/CurrencyDataStruct.h"
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

	InitializeDataTables();
}

void UKRDataTablesSubsystem::Deinitialize()
{
	Super::Deinitialize();
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
	TObjectPtr<UCacheDataTable>* CacheDataTable = DataTables.Find(InStruct);
	if (!CacheDataTable || !(*CacheDataTable))
	{
		return nullptr;
	}

	return *CacheDataTable;
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
	AddDataTable<FEnemyAttributeDataStruct>(FString(TEXT("EnemyAttributeData")));
	AddDataTable<FDialogueDataStruct>(FString(TEXT("DialogueData")));

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
