#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "QuestStateTreeCondition_SubQuestComplete.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (
	DisplayName = "SubQuest Complete",
	category = "KRQuest"
	)
)
class KORAPROJECT_API UQuestStateTreeCondition_SubQuestComplete : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

public:
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context)
	TObjectPtr<AActor> Actor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	int32 Order;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bInvert;

#if WITH_EDITOR

	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override
	{
		return NSLOCTEXT("QuestStateTreeCondition_SubQuestComplete", "SubQuest Complete", "Check SubQuest Complete");
	}
	
#endif
	
};
