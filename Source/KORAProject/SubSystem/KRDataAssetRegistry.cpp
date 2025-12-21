#include "KRDataAssetRegistry.h"
#include "KRDataTablesSubsystem.h"
#include "KRSoundSubsystem.h"
#include "KREffectSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Data/GameDataType.h"
#include "Data/KRDataAssetTableRows.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY(LogKRDataAssetRegistry);

void UKRDataAssetRegistry::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bIsInitialized = true;
}

void UKRDataAssetRegistry::Deinitialize()
{
	WorldEventDataMap.Empty();
	RegisteredSoundCount = 0;
	RegisteredEffectCount = 0;
	RegisteredWorldEventCount = 0;
	bIsInitialized = false;
	Super::Deinitialize();
}

FKRDataAssetRegistrationResult UKRDataAssetRegistry::RegisterAllFromDataTablesSubsystem()
{
	FKRDataAssetRegistrationResult Result;

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("RegisterAllFromDataTablesSubsystem: GameInstance is null"));
		return Result;
	}

	UKRDataTablesSubsystem* DataTablesSubsystem = GameInstance->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataTablesSubsystem)
	{
		UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("RegisterAllFromDataTablesSubsystem: DataTablesSubsystem is null"));
		return Result;
	}

	if (UCacheDataTable* SoundCacheTable = DataTablesSubsystem->GetTable(EGameDataType::SoundDefinitionData))
	{
		if (UDataTable* SoundTable = SoundCacheTable->GetTable())
		{
			Result.SoundDefinitionsRegistered = RegisterSoundDefinitionsFromTable(SoundTable);
		}
	}

	if (UCacheDataTable* EffectCacheTable = DataTablesSubsystem->GetTable(EGameDataType::EffectDefinitionData))
	{
		if (UDataTable* EffectTable = EffectCacheTable->GetTable())
		{
			Result.EffectDefinitionsRegistered = RegisterEffectDefinitionsFromTable(EffectTable);
		}
	}

	if (UCacheDataTable* WorldEventCacheTable = DataTablesSubsystem->GetTable(EGameDataType::WorldEventData))
	{
		if (UDataTable* WorldEventTable = WorldEventCacheTable->GetTable())
		{
			Result.WorldEventsRegistered = RegisterWorldEventsFromTable(WorldEventTable);
		}
	}

	Result.TotalRegistered = Result.SoundDefinitionsRegistered + Result.EffectDefinitionsRegistered + Result.WorldEventsRegistered;
	return Result;
}

int32 UKRDataAssetRegistry::RegisterSoundDefinitionsFromTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("RegisterSoundDefinitionsFromTable: DataTable is null"));
		return 0;
	}

	if (DataTable->GetRowStruct() != FSoundDefinitionDataStruct::StaticStruct())
	{
		UE_LOG(LogKRDataAssetRegistry, Error, TEXT("RegisterSoundDefinitionsFromTable: Invalid row structure. Expected FSoundDefinitionDataStruct"));
		return 0;
	}

	int32 RegisteredCount = 0;
	TArray<FName> RowNames = DataTable->GetRowNames();
	
	for (const FName& RowName : RowNames)
	{
		FSoundDefinitionDataStruct* Row = DataTable->FindRow<FSoundDefinitionDataStruct>(RowName, TEXT(""));
		if (!Row)
		{
			continue;
		}

		if (!Row->bEnabled)
		{
			UE_LOG(LogKRDataAssetRegistry, Verbose, TEXT("  [SKIP] %s - Disabled"), *RowName.ToString());
			continue;
		}

		if (Row->SoundDefinition.IsNull())
		{
			UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("  [FAIL] %s - SoundDefinition is null"), *RowName.ToString());
			continue;
		}

		UKRSoundDefinition* SoundDef = Row->SoundDefinition.LoadSynchronous();
		if (!SoundDef)
		{
			UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("  [FAIL] %s - Failed to load SoundDefinition"), *RowName.ToString());
			continue;
		}

		if (RegisterSoundDefinition(SoundDef))
		{
			RegisteredCount++;
			UE_LOG(LogKRDataAssetRegistry, Verbose, TEXT("  [OK] %s - %s"), *RowName.ToString(), *SoundDef->SoundTag.ToString());
		}
	}

	RegisteredSoundCount = RegisteredCount;
	return RegisteredCount;
}

int32 UKRDataAssetRegistry::RegisterEffectDefinitionsFromTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("RegisterEffectDefinitionsFromTable: DataTable is null"));
		return 0;
	}
	
	if (DataTable->GetRowStruct() != FEffectDefinitionDataStruct::StaticStruct())
	{
		UE_LOG(LogKRDataAssetRegistry, Error, TEXT("RegisterEffectDefinitionsFromTable: Invalid row structure. Expected FEffectDefinitionDataStruct"));
		return 0;
	}

	int32 RegisteredCount = 0;
	TArray<FName> RowNames = DataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FEffectDefinitionDataStruct* Row = DataTable->FindRow<FEffectDefinitionDataStruct>(RowName, TEXT(""));
		if (!Row)
		{
			continue;
		}

		if (!Row->bEnabled)
		{
			UE_LOG(LogKRDataAssetRegistry, Verbose, TEXT("  [SKIP] %s - Disabled"), *RowName.ToString());
			continue;
		}

		if (Row->EffectDefinition.IsNull())
		{
			UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("  [FAIL] %s - EffectDefinition is null"), *RowName.ToString());
			continue;
		}
		
		UKREffectDefinition* EffectDef = Row->EffectDefinition.LoadSynchronous();
		if (!EffectDef)
		{
			UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("  [FAIL] %s - Failed to load EffectDefinition"), *RowName.ToString());
			continue;
		}

		if (RegisterEffectDefinition(EffectDef))
		{
			RegisteredCount++;
		}
	}

	RegisteredEffectCount = RegisteredCount;
	return RegisteredCount;
}

int32 UKRDataAssetRegistry::RegisterWorldEventsFromTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("RegisterWorldEventsFromTable: DataTable is null"));
		return 0;
	}
	
	if (DataTable->GetRowStruct() != FWorldEventDataStruct::StaticStruct())
	{
		UE_LOG(LogKRDataAssetRegistry, Error, TEXT("RegisterWorldEventsFromTable: Invalid row structure. Expected FWorldEventDataStruct"));
		return 0;
	}

	int32 RegisteredCount = 0;
	TArray<FName> RowNames = DataTable->GetRowNames();
	
	for (const FName& RowName : RowNames)
	{
		FWorldEventDataStruct* Row = DataTable->FindRow<FWorldEventDataStruct>(RowName, TEXT(""));
		if (!Row)
		{
			continue;
		}

		if (!Row->bEnabled)
		{
			UE_LOG(LogKRDataAssetRegistry, Verbose, TEXT("  [SKIP] %s - Disabled"), *RowName.ToString());
			continue;
		}

		if (Row->WorldEventData.IsNull())
		{
			UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("  [FAIL] %s - WorldEventData is null"), *RowName.ToString());
			continue;
		}
		
		UKRWorldEventData* WorldEventData = Row->WorldEventData.LoadSynchronous();
		if (!WorldEventData)
		{
			UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("  [FAIL] %s - Failed to load WorldEventData"), *RowName.ToString());
			continue;
		}
		
		if (WorldEventData->EventTag.IsValid())
		{
			WorldEventDataMap.Add(WorldEventData->EventTag, WorldEventData);
			RegisteredCount++;
		}
		else
		{
			UE_LOG(LogKRDataAssetRegistry, Warning, TEXT("  [FAIL] %s - Invalid EventTag"), *RowName.ToString());
		}
	}

	RegisteredWorldEventCount = RegisteredCount;
	return RegisteredCount;
}

bool UKRDataAssetRegistry::RegisterSoundDefinition(UKRSoundDefinition* SoundDef)
{
	if (!SoundDef || !SoundDef->SoundTag.IsValid())
	{
		return false;
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	UWorld* World = GameInstance->GetWorld();
	if (!World)
	{
		return false;
	}

	UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>();
	if (!SoundSubsystem)
	{
		return false;
	}

	SoundSubsystem->RegisterSoundDefinition(SoundDef);
	return true;
}

bool UKRDataAssetRegistry::RegisterEffectDefinition(UKREffectDefinition* EffectDef)
{
	if (!EffectDef || !EffectDef->EffectTag.IsValid())
	{
		return false;
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	UWorld* World = GameInstance->GetWorld();
	if (!World)
	{
		return false;
	}

	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (!EffectSubsystem)
	{
		return false;
	}

	EffectSubsystem->RegisterEffectDefinition(EffectDef);
	return true;
}

UKRWorldEventData* UKRDataAssetRegistry::GetWorldEventData(const FGameplayTag& EventTag) const
{
	if (const TObjectPtr<UKRWorldEventData>* Found = WorldEventDataMap.Find(EventTag))
	{
		return *Found;
	}
	return nullptr;
}
