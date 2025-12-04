#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "QuestStateTreeTask_SubQuest.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestStateTreeTask_SubQuest, Log, All);

USTRUCT(BlueprintType)
struct KORAPROJECT_API FQuestStateTreeTask_SubQuestInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context)
	TObjectPtr<AActor> Actor;	
	
	UPROPERTY(EditAnywhere, Category = Input)
	int32 Order = 0;

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
USTRUCT(meta = (
	DisplayName = "SubQuest Task",
	category = "KRQuest"
	)
)
struct KORAPROJECT_API FQuestStateTreeTask_SubQuest : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceData = FQuestStateTreeTask_SubQuestInstanceData;

	FQuestStateTreeTask_SubQuest() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR

	virtual FName GetIconName() const override { return FName("GenericPlay"); }
	virtual FColor GetIconColor() const override { return FColor::Green; }
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override
	{
		return NSLOCTEXT("QuestStateTreeTask_SubQuest", "Enter SubQuest", "Enter SubQuest");
	}
	
#endif
	
private:
	UPROPERTY()
	mutable FString DebugFailReason;
};
