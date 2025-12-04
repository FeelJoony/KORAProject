#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeExecutionTypes.h"
#include "QuestStateTreeEvaluator_SubQuestProgress.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestStateTreeEvaluator_SubQuestProgress, Log, All);

UCLASS(Blueprintable, meta =
	(
		DisplayName = "SubQuest Progress Evaluator",
		category = "KRQuest",
		Icon = "Check_Circle"
	)
)
class KORAPROJECT_API UQuestStateTreeEvaluator_SubQuestProgress : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	virtual void TreeStart(FStateTreeExecutionContext& Context) override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	virtual void TreeStop(FStateTreeExecutionContext& Context) override;

	void Reset();

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override
	{
		return NSLOCTEXT("QuestStateTreeEvaluator_SubQuestProgress", "Evaluate Sub Quest Order Index", "SubQuest Progress Evaluator");
	}
	
#endif
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context)
	TObjectPtr<AActor> Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
	int32 CurrentOrder;
};
