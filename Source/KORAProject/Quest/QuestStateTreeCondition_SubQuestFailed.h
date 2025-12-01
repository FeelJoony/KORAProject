#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "QuestStateTreeCondition_SubQuestFailed.generated.h"

USTRUCT()
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestFailedInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 SubQuestDataKey = 0;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag SubQuestTypeTag;

	UPROPERTY(EditAnywhere, Category = Context)
	FStateTreeStructRef EvaluatorDataRef;

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bInvert = false;
};

USTRUCT(meta = (DisplayName = "SubQuest Failed Condition"))
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestFailed : public FStateTreeConditionBase
{
	GENERATED_BODY()

	using FInstanceData = FQuestStateTreeCondition_SubQuestFailedInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
