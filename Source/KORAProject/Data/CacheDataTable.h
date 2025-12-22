#pragma once

#include "CoreMinimal.h"
#include "Data/GameDataType.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "CacheDataTable.generated.h"

UCLASS()
class KORAPROJECT_API UCacheDataTable : public UObject
{
	GENERATED_BODY()

public:
	void Init(EGameDataType InDataType, UDataTable* NewDataTable);

	FORCEINLINE UDataTable* GetTable() const
	{
		return CachedDataTable;
	}

	FORCEINLINE const FName& GetKeyName(int32 InKey) const
	{
		return KeyList[InKey];
	}

	FORCEINLINE const FName& GetKeyName(FGameplayTag InKey) const
	{
		return KeyList[GetTypeHash(InKey)];
	}

	FORCEINLINE bool ContainsKey(uint32 InKey) const
	{
		return KeyList.Contains(InKey);
	}

	FORCEINLINE bool ContainsKey(FGameplayTag InKey) const
	{
		return KeyList.Contains(GetTypeHash(InKey));
	}

	template<typename TRow>
	TRow* FindRow(uint32 InKey, const FString& ContextString, bool bWarnIfRowMissing)
	{
		return CachedDataTable->FindRow<TRow>(KeyList[InKey], ContextString, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* FindRow(FGameplayTag InKey, const FString& ContextString, bool bWarnIfRowMissing)
	{
		return CachedDataTable->FindRow<TRow>(KeyList[GetTypeHash(InKey)], ContextString, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* FindRowSafe(uint32 InKey, const FString& ContextString, bool bWarnIfRowMissing)
	{
		if (!ContainsKey(InKey))
		{
			return nullptr;
		}
		return CachedDataTable->FindRow<TRow>(KeyList[InKey], ContextString, bWarnIfRowMissing);
	}

	template<typename TRow>
	TRow* FindRowSafe(FGameplayTag InKey, const FString& ContextString, bool bWarnIfRowMissing)
	{
		if (!ContainsKey(InKey))
		{
			return nullptr;
		}
		return CachedDataTable->FindRow<TRow>(KeyList[GetTypeHash(InKey)], ContextString, bWarnIfRowMissing);
	}

	FORCEINLINE void GetKeys(TArray<uint32>& OutKeys) const
	{
		KeyList.GetKeys(OutKeys);
	}

private:
	UPROPERTY()
	TMap<uint32, FName> KeyList;
	UPROPERTY()
	TObjectPtr<UDataTable> CachedDataTable;

	template<typename TRow>
	void InitKeyList(UDataTable* NewDataTable)
	{
		static_assert(std::is_base_of_v<class ITableKey, TRow>, "TRow must be a child of ITableKey");

		CachedDataTable = NewDataTable;

		if (!NewDataTable)
		{
			return;
		}

		const TArray<FName>& RowNames = NewDataTable->GetRowNames();
		for (const auto& RowName : RowNames)
		{
			const TRow* Row = reinterpret_cast<const TRow*>(NewDataTable->FindRowUnchecked(RowName));

			KeyList.Add(Row->GetKey(), RowName);
		}
	}
};
