#pragma once

#include "CoreMinimal.h"
#include "QuestConditionChecker.h"
#include "PlayAbilityChecker.generated.h"

UCLASS()
class KORAPROJECT_API UPlayAbilityChecker : public UQuestConditionChecker
{
	GENERATED_BODY()

public:
	UPlayAbilityChecker();
	
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;
};
