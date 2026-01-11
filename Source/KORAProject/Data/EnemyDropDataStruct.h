#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "EnemyDropDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FDropItemEntry
{
	GENERATED_BODY()

	FDropItemEntry()
		: ItemTag(FGameplayTag())
		, MinCount(1)
		, MaxCount(1)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DropItem)
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DropItem)
	int32 MinCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DropItem)
	int32 MaxCount;
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FEnemyDropDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FEnemyDropDataStruct()
		: EnemyTag(FGameplayTag())
		, GearingMin(0)
		, GearingMax(0)
		, CorbyteChanceMin(0.f)
		, CorbyteChanceMax(0.f)
		, CorbyteAmount(1)
		, GuaranteedItemCount(0)
		, CommonChance(0.f)
		, UncommonChance(0.f)
		, RareChance(0.f)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	FGameplayTag EnemyTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	int32 GearingMin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	int32 GearingMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	float CorbyteChanceMin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	float CorbyteChanceMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	int32 CorbyteAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	int32 GuaranteedItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	float CommonChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	float UncommonChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	float RareChance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyDrop)
	TArray<FDropItemEntry> DroppableItems;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(EnemyTag);
	}
};
