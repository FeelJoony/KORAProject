#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "KRAIStateTree_EventPayloadData.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRAIStateTree_EventPayloadData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Payload")
	FGameplayTag ValueTag;
};
