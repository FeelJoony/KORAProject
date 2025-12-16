#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Data/GameDataType.h"

UKRDataTablesSubsystem::UKRDataTablesSubsystem()
{

}

void UKRDataTablesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AddDataTable(EGameDataType::SampleData, FString(TEXT("SampleData")));
	AddDataTable(EGameDataType::ItemData, FString(TEXT("ItemData")));
	AddDataTable(EGameDataType::QuestData, FString(TEXT("QuestData")));
	AddDataTable(EGameDataType::SubQuestData, FString(TEXT("SubQuestData")));
	AddDataTable(EGameDataType::WeaponData, FString(TEXT("WeaponData")));
	AddDataTable(EGameDataType::WeaponEnhanceData, FString(TEXT("WeaponEnhanceData")));
	AddDataTable(EGameDataType::TutorialData, FString(TEXT("TutorialData")));
	AddDataTable(EGameDataType::ShopItemData, FString(TEXT("ShopItemData")));
	AddDataTable(EGameDataType::EquipmentData, FString(TEXT("EquipmentData")));
	AddDataTable(EGameDataType::ConsumeData, FString(TEXT("ConsumeData")));
	AddDataTable(EGameDataType::EquipData, FString(TEXT("EquipData")));
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
