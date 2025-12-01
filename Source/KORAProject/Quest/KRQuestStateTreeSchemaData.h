#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KRQuestStateTreeSchemaData.generated.h"

UCLASS()
class KORAPROJECT_API UKRQuestStateTreeSchemaData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest|StateTreeSchema", meta=(AllowedTypes="KRQuestDataDefinition"))
	FPrimaryAssetId StateTreeSchemaID;
};
