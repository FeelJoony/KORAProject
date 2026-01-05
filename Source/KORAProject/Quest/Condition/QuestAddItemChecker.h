#pragma once

#include "CoreMinimal.h"
#include "QuestConditionChecker.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "QuestAddItemChecker.generated.h"

struct FAddItemMessage;

UCLASS()
class KORAPROJECT_API UQuestAddItemChecker : public UQuestConditionChecker
{
	GENERATED_BODY()

public:
	UQuestAddItemChecker();

	virtual UQuestConditionChecker* Initialize(class UKRQuestInstance* NewQuestInstance, const struct FSubQuestEvalDataStruct& EvalData) override;
	virtual void Uninitialize() override;

protected:
	virtual bool CanCount(const struct FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;

private:
	void OnAddItemMessage(FGameplayTag Channel, const FAddItemMessage& Message);

	FGameplayMessageListenerHandle ListenerHandle;
};
