#pragma once

#include "GameplayTagContainer.h"
#include "KRItemUIData.generated.h"

class UTexture2D;
// class UKRBaseItem;

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRItemUIData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere) FName ItemID = NAME_None;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) FText ItemName;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) FText ItemDescription;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) TSoftObjectPtr<UTexture2D> ItemIcon;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 Quantity = 0;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 Price = 0;
    UPROPERTY(BlueprintReadOnly, EditAnywhere) int32 UpgradeLevel = 0;
    //UPROPERTY(BlueprintReadOnly, EditAnywhere) FGameplayTagContainer Tags;

    //UPROPERTY() TWeakObjectPtr<UKRBaseItem> SourceItem;
};