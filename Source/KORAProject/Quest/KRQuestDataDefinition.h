#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KRQuestDataDefinition.generated.h"

UCLASS()
class KORAPROJECT_API UKRQuestDataDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UKRQuestDataDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = QuestData)
	TObjectPtr<class UStateTree> QuestStateTree;
};
