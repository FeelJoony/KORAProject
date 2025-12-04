#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionTypes.h"
#include "Components/StateTreeComponentSchema.h"
#include "QuestStateTreeSchema.generated.h"

/**
 * Quest System용 StateTree Schema
 * - Context로 QuestComponent와 QuestInstance를 자동 제공
 * - Quest 전용 Tasks/Conditions/Evaluators만 허용
 */
UCLASS(BlueprintType, EditInlineNew, CollapseCategories, meta = (DisplayName = "Quest StateTree Schema"))
class KORAPROJECT_API UQuestStateTreeSchema : public UStateTreeComponentSchema
{
	GENERATED_BODY()

public:
	UQuestStateTreeSchema();

	// UStateTreeComponentSchema가 자동으로 Owner Actor의 컴포넌트를 Context로 제공
	// GetContextDataDescs()를 오버라이드하면 안 됨!

	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;
	
protected:
	// Quest 전용 노드만 허용하도록 필터링
	virtual bool IsStructAllowed(const UScriptStruct* InStruct) const override;
	virtual bool IsClassAllowed(const UClass* InClass) const override;
	virtual bool IsExternalItemAllowed(const UStruct& InStruct) const override;


private:
	static TArray<FStateTreeExternalDataDesc> ContextDataDescs;
};
