#pragma once

#include "CoreMinimal.h"
#include "QuestConditionChecker.h"
#include "QuestAddItemChecker.generated.h"

UCLASS()
class KORAPROJECT_API UQuestAddItemChecker : public UQuestConditionChecker
{
	GENERATED_BODY()

public:
	UQuestAddItemChecker();


protected:
	virtual bool CanCount(const struct FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;
};
