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

	virtual void Initialize(FGameplayTag InTag) {}
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag);
	
	
protected:
	UPROPERTY()
	FGameplayTag Tag;
	
};
