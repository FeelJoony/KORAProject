#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/CacheDataTable.h"
#include "KRDataTablesSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDataTablesSubsystem, Log, All);

UCLASS()
class KORAPROJECT_API UKRDataTablesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UKRDataTablesSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UKRDataTablesSubsystem& Get(const UObject* WorldContextObject);
	static UKRDataTablesSubsystem* GetSafe(const UObject* WorldContextObject);
	
	UCacheDataTable* GetTable(UScriptStruct* InStruct);

	template<typename TRow>
	TRow* GetData(uint32 InKey, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = true)
	{
		return GetTable(TRow::StaticStruct())->FindRow<TRow>(InKey, StringContext, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* GetData(FGameplayTag KeyTag, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = true)
	{
		return GetTable(TRow::StaticStruct())->FindRow<TRow>(KeyTag, StringContext, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* GetDataSafe(FGameplayTag KeyTag, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = false)
	{
		UCacheDataTable* Table = GetTable(TRow::StaticStruct());
		if (!Table || !Table->ContainsKey(KeyTag))
		{
			return nullptr;
		}
		return Table->FindRow<TRow>(KeyTag, StringContext, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* GetDataSafe(uint32 InKey, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = false)
	{
		UCacheDataTable* Table = GetTable(TRow::StaticStruct());
		if (!Table || !Table->ContainsKey(InKey))
		{
			return nullptr;
		}
		return Table->FindRow<TRow>(InKey, StringContext, bWarnIfRowMissing);
	}

	UFUNCTION(BlueprintCallable, Category = "DataTables|Validation")
	void ValidateDataReferences();

	void InitializeDataTables();
	
	const struct FEquipDataStruct* GetEquipDataByItemTag(const FGameplayTag& ItemTag);
	const struct FModuleDataStruct* GetModuleDataByItemTag(const FGameplayTag& ItemTag);

private:
	UPROPERTY()
	TMap<TObjectPtr<UScriptStruct>, TObjectPtr<UCacheDataTable>> DataTables;

	const FString TablePath = TEXT("/Game/Data/DataTables");
	const FString DefaultStringContext = TEXT("");

	template<typename TRow>
	void AddDataTable(const FString& TableName);
};

template <typename TRow>
void UKRDataTablesSubsystem::AddDataTable(const FString& TableName)
{
	UDataTable* DataTable = Cast<class UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *FPaths::Combine(*TablePath, *FString::Printf(TEXT("%s.%s"), *TableName, *TableName))));

	if (!DataTable)
	{
		UE_LOG(LogDataTablesSubsystem, Warning, TEXT("DataTable not found: %s"), *TableName);
		return;
	}

	UCacheDataTable* CacheTable = NewObject<UCacheDataTable>(this);
	CacheTable->InitKeyList<TRow>(DataTable);

	DataTables.Add(TRow::StaticStruct(), CacheTable);
}
