#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/CacheDataTable.h"
#include "KRDataTablesSubsystem.generated.h"

UCLASS()
class KORAPROJECT_API UKRDataTablesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UKRDataTablesSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UKRDataTablesSubsystem& Get(const UObject* WorldContextObject);
	static UKRDataTablesSubsystem* GetSafe();
	
	UCacheDataTable* GetTable(EGameDataType InDataType);

	template<typename TRow>
	TRow* GetData(EGameDataType InType, uint32 InKey, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = true)
	{
		return GetTable(InType)->FindRow<TRow>(InKey, StringContext, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* GetData(EGameDataType InType, FGameplayTag KeyTag, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = true)
	{
		return GetTable(InType)->FindRow<TRow>(KeyTag, StringContext, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* GetDataSafe(EGameDataType InType, FGameplayTag KeyTag, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = false)
	{
		UCacheDataTable* Table = GetTable(InType);
		if (!Table || !Table->ContainsKey(KeyTag))
		{
			return nullptr;
		}
		return Table->FindRow<TRow>(KeyTag, StringContext, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* GetDataSafe(EGameDataType InType, uint32 InKey, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = false)
	{
		UCacheDataTable* Table = GetTable(InType);
		if (!Table || !Table->ContainsKey(InKey))
		{
			return nullptr;
		}
		return Table->FindRow<TRow>(InKey, StringContext, bWarnIfRowMissing);
	}

	UFUNCTION(BlueprintCallable, Category = "DataTables|Validation")
	void ValidateDataReferences();
	
	const struct FEquipDataStruct* GetEquipDataByItemTag(const FGameplayTag& ItemTag);
	const struct FModuleDataStruct* GetModuleDataByItemTag(const FGameplayTag& ItemTag);

private:
	UPROPERTY()
	TMap<EGameDataType, TSoftObjectPtr<UCacheDataTable>> DataTables;

	const FString TablePath = TEXT("/Game/Data/DataTables");
	const FString DefaultStringContext = TEXT("");

	// 테이블 타입별 이름 매핑 (Initialize에서 초기화)
	TMap<EGameDataType, FString> TableNames;

	void AddDataTable(EGameDataType InType, const FString& TableName);
	void InitializeTableNames();

};
