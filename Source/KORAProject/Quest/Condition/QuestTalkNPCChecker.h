#pragma once

#include "CoreMinimal.h"
#include "QuestConditionChecker.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "QuestTalkNPCChecker.generated.h"

USTRUCT(BlueprintType)
struct FQuestNPCDialogueCompleteMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FGameplayTag NPCTag;
};

UCLASS()
class KORAPROJECT_API UQuestTalkNPCChecker : public UQuestConditionChecker
{
	GENERATED_BODY()

public:
	UQuestTalkNPCChecker();

	virtual UQuestConditionChecker* Initialize(class UKRQuestInstance* NewQuestInstance, const struct FSubQuestEvalDataStruct& EvalData) override;
	virtual void Uninitialize() override;

protected:
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;

	UFUNCTION()
	void OnNPCDialogueComplete(FGameplayTag Channel, const FQuestNPCDialogueCompleteMessage& Payload);

private:
	FGameplayMessageListenerHandle ListenerHandle;
};
