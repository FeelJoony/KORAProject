#pragma once

#include "CoreMinimal.h"
#include "KRBaseItemData.h"
#include "KRConsumeItemData.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRConsumeItemData : public FKRBaseItemData
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Tag")
	FGameplayTag PoolTag;
};