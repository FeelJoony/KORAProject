#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Data/SubQuestDataStruct.h"
#include "QuestConditionChecker.generated.h"

UCLASS()
class KORAPROJECT_API UQuestConditionChecker : public UObject
{
	GENERATED_BODY()

public:
	UQuestConditionChecker();

	virtual UQuestConditionChecker* Initialize(class UKRQuestInstance* NewQuestInstance, const struct FSubQuestEvalDataStruct& EvalData) { return nullptr; }
	virtual void Uninitialize() {}
	
protected:
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	TObjectPtr<class UKRQuestInstance> QuestInstance;
	
	FGameplayTag ObjectiveTag;
};
