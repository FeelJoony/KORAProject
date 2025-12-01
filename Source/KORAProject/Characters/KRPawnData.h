#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KRPawnData.generated.h"

UCLASS()
class KORAPROJECT_API UKRPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UKRPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Pawn")
	TSubclassOf<APawn> PawnClass;

};
