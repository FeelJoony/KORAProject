#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KRExperienceActionSet.generated.h"

UCLASS()
class KORAPROJECT_API UKRExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UKRExperienceActionSet();

	UPROPERTY(EditAnywhere, Category = "Actions to Perform")
	TArray<TObjectPtr<class UGameFeatureAction>> Actions;
};
