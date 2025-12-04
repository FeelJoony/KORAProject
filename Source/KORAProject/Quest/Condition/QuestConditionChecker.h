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
	
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag, UStruct* InStruct);
	
	
protected:
	UPROPERTY()
	FGameplayTag Tag;
	
};
