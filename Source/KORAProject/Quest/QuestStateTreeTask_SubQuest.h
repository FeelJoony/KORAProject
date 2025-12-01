#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "QuestStateTreeTask_SubQuest.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestStateTreeTask_SubQuest, Log, All);

/**
 * SubQuest Task Instance Data
 * - SubQuestTag로 SubQuest를 식별 (DataTable의 SubQuestTypeTag와 매칭)
 * - Schema의 Context에서 QuestComponent 자동 주입
 */
USTRUCT(BlueprintType)
struct KORAPROJECT_API FQuestStateTreeTask_SubQuestInstanceData
{
	GENERATED_BODY()

	// SubQuest를 식별하는 GameplayTag (예: Quest.Type.Kill.Enemy)
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (Categories = "Quest.Type"))
	FGameplayTag SubQuestTag;

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 Order;

	// 내부 상태
	UPROPERTY()
	bool bIsInitialized = false;
};

/**
 * SubQuest 실행 Task
 * - SubQuestTag로 DataTable에서 SubQuest 정보 로드
 * - Evaluator에 SubQuest 등록 및 활성화
 * - 완료 조건 체크하여 State 전환
 */
USTRUCT(meta = (DisplayName = "SubQuest Task"))
struct KORAPROJECT_API FQuestStateTreeTask_SubQuest : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceData = FQuestStateTreeTask_SubQuestInstanceData;

	FQuestStateTreeTask_SubQuest() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) const override;

	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual bool Link(FStateTreeLinker& Linker) override;

protected:
	// External Data Handles - Link()에서 자동 연결됨
	TStateTreeExternalDataHandle<struct FQuestStateTreeEvaluator_SubQuestProgressInstanceData> ProgressEvaluatorHandle;

	// Progress Evaluator Instance Data 가져오기
	struct FQuestStateTreeEvaluator_SubQuestProgressInstanceData* GetEvaluatorInstanceData(FStateTreeExecutionContext& Context) const;

private:
	UPROPERTY()
	mutable FString DebugFailReason;
};
