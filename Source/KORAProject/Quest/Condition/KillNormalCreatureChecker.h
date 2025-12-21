#pragma once

#include "CoreMinimal.h"
#include "QuestConditionChecker.h"
#include "KillNormalCreatureChecker.generated.h"

UCLASS()
class KORAPROJECT_API UKillNormalCreatureChecker : public UQuestConditionChecker
{
	GENERATED_BODY()

public:
	UKillNormalCreatureChecker();

	virtual UQuestConditionChecker* Initialize(class UKRQuestInstance* NewQuestInstance, const struct FSubQuestEvalDataStruct& EvalData) override;
	virtual void Uninitialize() override;
	
protected:
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;
	
};
