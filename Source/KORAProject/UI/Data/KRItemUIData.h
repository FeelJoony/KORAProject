#pragma once

#include "KRItemUIData.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRItemUIData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere) FName ItemNameKey;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) FName ItemDescriptionKey;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) TSoftObjectPtr<UTexture2D> ItemIcon;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 Quantity = 0;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 Price = -1;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 UpgradeLevel = 0;
};