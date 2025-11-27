#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ShopItemData.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FShopItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop Item")
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop Item")
	FGameplayTag PoolTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop Item", meta = (ClampMin = "1"))
	int32 BaseStock = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop Item", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;
};