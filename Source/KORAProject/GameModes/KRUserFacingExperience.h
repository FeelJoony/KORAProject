#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KRUserFacingExperience.generated.h"

UCLASS()
class KORAPROJECT_API UKRUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="Map"))
	FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="KRExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
