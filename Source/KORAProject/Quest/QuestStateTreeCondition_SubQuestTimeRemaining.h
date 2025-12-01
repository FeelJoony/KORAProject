#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "QuestStateTreeCondition_SubQuestTimeRemaining.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestTimeRemainingInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 SubQuestDataKey = 0;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag SubQuestTypeTag;

	UPROPERTY(EditAnywhere, Category = Context)
	FStateTreeStructRef EvaluatorDataRef;

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "0.0"))
	float TimeThreshold = 10.f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bCheckIfLessThan = true;
};

USTRUCT(meta = (DisplayName = "SubQuest Time Remaining Condition"))
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestTimeRemaining : public FStateTreeConditionBase  
{
	GENERATED_BODY()
	
	using FInstanceData = FQuestStateTreeCondition_SubQuestTimeRemainingInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
