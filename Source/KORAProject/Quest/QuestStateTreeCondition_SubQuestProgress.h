#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "QuestStateTreeCondition_SubQuestProgress.generated.h"

UENUM(BlueprintType)
enum class EProgressComparisonOp : uint8
{
	GreaterThan				UMETA(DisplayName = ">"),
	GreaterThanOrEqual		UMETA(DisplayName = ">="),
	LessThan				UMETA(DisplayName = "<"),
	LessThanOrEqual			UMETA(DisplayName = "<="),
	Equal					UMETA(DisplayName = "=="),
	NotEqual				UMETA(DisplayName = "!=")
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestProgressInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 SubQuestDataKey = 0;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag SubQuestTypeTag;

	UPROPERTY(EditAnywhere, Category = Context)
	FStateTreeStructRef EvaluatorDataRef;

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bUseProgressRatio = false;

	UPROPERTY(EditAnywhere, Category = Parameter)
	EProgressComparisonOp ComparisonOp = EProgressComparisonOp::GreaterThanOrEqual;

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "0"))
	float ComparisonValue = 1.0f;
	
};

USTRUCT(meta = (DisplayName = "SubQuest Progress Condition"))
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestProgress : public FStateTreeConditionBase
{
	GENERATED_BODY()

	using FInstanceData = FQuestStateTreeCondition_SubQuestProgressInstanceData;
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
