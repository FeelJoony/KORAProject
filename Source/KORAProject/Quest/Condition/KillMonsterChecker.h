#pragma once

#include "CoreMinimal.h"
#include "Quest/Condition/QuestConditionChecker.h"
#include "KillMonsterChecker.generated.h"

UCLASS()
class KORAPROJECT_API UKillMonsterChecker : public UQuestConditionChecker
{
	GENERATED_BODY()
	
public:
	UKillMonsterChecker();

	virtual UQuestConditionChecker* Initialize(class UKRQuestInstance* NewQuestInstance, const struct FSubQuestEvalDataStruct& EvalData) override;
	virtual void Uninitialize() override;

protected:
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;

	UFUNCTION()
	void ReceiveMessage(FGameplayTag InTag, const struct FKillMonsterMessage& Message);
};
