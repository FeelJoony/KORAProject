#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "GameplayTagContainer.h"
#include "QuestStateTreeCondition_SubQuestComplete.generated.h"

/**
 * SubQuest 완료 여부 체크 Condition
 * - SubQuestTag로 SubQuest 식별
 * - bInvert = true면 미완료 상태 체크
 */
USTRUCT(BlueprintType)
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestCompleteInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 Order;
};

/**
 * SubQuest 완료 조건
 * - Evaluator의 진행 상태에서 완료 여부 확인
 */
USTRUCT(meta = (DisplayName = "SubQuest Complete"))
struct KORAPROJECT_API FQuestStateTreeCondition_SubQuestComplete : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceData = FQuestStateTreeCondition_SubQuestCompleteInstanceData;

	FQuestStateTreeCondition_SubQuestComplete();

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual bool Link(FStateTreeLinker& Linker) override;

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
	// External Data Handles - Link()에서 자동 연결됨
	TStateTreeExternalDataHandle<struct FQuestStateTreeEvaluator_SubQuestProgressInstanceData> ProgressEvaluatorHandle;

	const struct FQuestStateTreeEvaluator_SubQuestProgressInstanceData* GetEvaluatorInstanceData(
		FStateTreeExecutionContext& Context) const;
};
