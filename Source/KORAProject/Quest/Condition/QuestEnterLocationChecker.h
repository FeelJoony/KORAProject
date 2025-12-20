#pragma once

#include "CoreMinimal.h"
#include "QuestConditionChecker.h"
#include "QuestEnterLocationChecker.generated.h"

UCLASS()
class KORAPROJECT_API UQuestEnterLocationChecker : public UQuestConditionChecker
{
	GENERATED_BODY()

public:
	UQuestEnterLocationChecker();

	virtual UQuestConditionChecker* Initialize(class UKRQuestInstance* NewQuestInstance, const struct FSubQuestEvalDataStruct& EvalData) override;
	virtual void Uninitialize() override;

protected:
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;

	UFUNCTION(Blueprintable)
	void ReceiveMessage(FGameplayTag Channel, const struct FQuestEventTriggerBoxMessage& Payload);
};
