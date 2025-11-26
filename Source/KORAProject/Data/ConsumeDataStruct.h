#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "ConsumeDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FConsumeDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()
	
	FConsumeDataStruct()
		:	GroupID(-1),
			StockBase(0),
			Weight(-1)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 GroupID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FGameplayTag PoolTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 StockBase;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float Weight;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(GroupID);
	}
};

