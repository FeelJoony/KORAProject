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

	virtual void Initialize(FGameplayTag InTag) override;
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;

	void Count(const FGameplayTag InTag, int32 NewCount);
};
