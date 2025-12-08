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

	virtual UQuestConditionChecker* Initialize(class UKRQuestInstance* NewQuestInstance, const struct FSubQuestEvalDataStruct& EvalData) override;
	virtual void Uninitialize() override;
	
protected:
	virtual bool CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag) override;


private:
	UPROPERTY()
	TObjectPtr<class APlayerState> PS;

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	FDelegateHandle OnTagCountChangedDelegateHandle;
};
