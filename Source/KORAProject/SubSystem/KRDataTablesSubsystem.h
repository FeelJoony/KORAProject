#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/CacheDataTable.h"
#include "KRDataTablesSubsystem.generated.h"

UCLASS()
class KORAPROJECT_API UKRDataTablesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UKRDataTablesSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UCacheDataTable* GetTable(EGameDataType InDataType);

	template<typename TRow>
	TRow* GetData(EGameDataType InType, int32 InKey, const FString& StringContext = FString(TEXT("")), bool bWarnIfRowMissing = true)
	{
		return GetTable(InType)->FindRow<TRow>(InKey, StringContext, bWarnIfRowMissing);
	}

private:
	UPROPERTY()
	TMap<EGameDataType, TSoftObjectPtr<UCacheDataTable>> DataTables;

	const FString TablePath = TEXT("/Game/Data/DataTables");
	const FString DefaultStringContext = TEXT("");

	void AddDataTable(EGameDataType InType, const FString& TableName);
	
};
