#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KRExperienceDefinition.generated.h"

UCLASS()
class KORAPROJECT_API UKRExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UKRExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = Experience)
	TObjectPtr<class UKRPawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, Category = Experience)
	TArray<FString> GameFeaturesToEnable;

	UPROPERTY(EditDefaultsOnly, Category = Experience)
	TArray<TObjectPtr<class UKRExperienceActionSet>> ActionSets;

	UPROPERTY(EditDefaultsOnly, Category = Experience)
	TArray<TObjectPtr<class UGameFeatureAction>> Actions;
};
