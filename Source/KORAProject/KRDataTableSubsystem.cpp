#include "KRDataTableSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Data/GameDataType.h"

UKRDataTableSubsystem::UKRDataTableSubsystem()
{

}

void UKRDataTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AddDataTable(EGameDataType::SampleData, FString(TEXT("SampleData")));
}

UCacheDataTable* UKRDataTableSubsystem::GetTable(EGameDataType InDataType)
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

void UKRDataTableSubsystem::AddDataTable(EGameDataType InType, const FString& TableName)
{
	UCacheDataTable* CacheTable = NewObject<UCacheDataTable>(this);
	class UDataTable* DataTable = Cast<class UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *FPaths::Combine(*TablePath, *FString::Printf(TEXT("%s.%s"), *TableName, *TableName))));

	CacheTable->Init(InType, DataTable);

	DataTables.Add(InType, CacheTable);
}
