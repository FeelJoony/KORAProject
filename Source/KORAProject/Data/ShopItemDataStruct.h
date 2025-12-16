#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "ShopItemDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FShopItemDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FShopItemDataStruct()
		: Index(0)
		, ItemTag(FGameplayTag())
		, PoolTag(FGameplayTag())
		, StockBase(-1)
		, Weight(0.f)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop Item")
	int32 Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop Item")
	FGameplayTag ItemTag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop Item")
	FGameplayTag PoolTag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop Item", meta = (ClampMin = "1"))
	int32 StockBase = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop Item", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(ItemTag);
	}
};