#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Data/GameDataType.h"
#include "Data/EquipDataStruct.h"
#include "Data/ModuleDataStruct.h"
#include "Data/EquipAbilityDataStruct.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDataTablesSubsystem, Log, All);
DEFINE_LOG_CATEGORY(LogDataTablesSubsystem);

UKRDataTablesSubsystem::UKRDataTablesSubsystem()
{

}

void UKRDataTablesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AddDataTable(EGameDataType::ItemData, FString(TEXT("ItemData")));
	AddDataTable(EGameDataType::WeaponEnhanceData, FString(TEXT("WeaponEnhanceData")));
	AddDataTable(EGameDataType::TutorialData, FString(TEXT("TutorialData")));
	AddDataTable(EGameDataType::ShopItemData, FString(TEXT("ShopItemData")));
	AddDataTable(EGameDataType::ConsumeData, FString(TEXT("ConsumeData")));
	AddDataTable(EGameDataType::QuestData, FString(TEXT("QuestData")));
	AddDataTable(EGameDataType::SubQuestData, FString(TEXT("SubQuestData")));
	AddDataTable(EGameDataType::EquipData, FString(TEXT("EquipData")));
	AddDataTable(EGameDataType::EquipAbilityData, FString(TEXT("EquipAbilityData")));
	AddDataTable(EGameDataType::ModuleData, FString(TEXT("ModuleData")));
	AddDataTable(EGameDataType::CurrencyData, FString(TEXT("CurrencyData")));
	AddDataTable(EGameDataType::SoundDefinitionData, FString(TEXT("SoundDefinitionData")));
	AddDataTable(EGameDataType::EffectDefinitionData, FString(TEXT("EffectDefinitionData")));
	AddDataTable(EGameDataType::WorldEventData, FString(TEXT("WorldEventData")));
	AddDataTable(EGameDataType::CitizenData, FString(TEXT("CitizenData")));

#if !UE_BUILD_SHIPPING
	ValidateDataReferences();
#endif
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

UCacheDataTable* UKRDataTablesSubsystem::GetTable(EGameDataType InDataType)
{
	TSoftObjectPtr<UCacheDataTable>& CacheDataTable = DataTables[InDataType];
	if (!CacheDataTable)
	{
		return nullptr;
	}

	if (CacheDataTable.IsPending())
	{
		CacheDataTable.LoadSynchronous();
	}

	return CacheDataTable.Get();
}

void UKRDataTablesSubsystem::AddDataTable(EGameDataType InType, const FString& TableName)
{
	UCacheDataTable* CacheTable = NewObject<UCacheDataTable>(this);
	UDataTable* DataTable = Cast<class UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *FPaths::Combine(*TablePath, *FString::Printf(TEXT("%s.%s"), *TableName, *TableName))));

	CacheTable->Init(InType, DataTable);

	DataTables.Add(InType, CacheTable);
}

void UKRDataTablesSubsystem::ValidateDataReferences()
{
	int32 ErrorCount = 0;

	UCacheDataTable* EquipTable = GetTable(EGameDataType::EquipData);
	UCacheDataTable* ModuleTable = GetTable(EGameDataType::ModuleData);
	UCacheDataTable* AbilityTable = GetTable(EGameDataType::EquipAbilityData);

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
