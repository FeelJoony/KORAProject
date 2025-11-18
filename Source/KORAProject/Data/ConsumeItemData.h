#pragma once

#include "CoreMinimal.h"
#include "BaseItemData.h"
#include "ConsumeItemData.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FConsumeItemData : public FBaseItemData
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Tag")
	FGameplayTag PoolTag;
};