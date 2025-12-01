#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionTypes.h"
#include "StateTreeSchema.h"
#include "QuestStateTreeSchema.generated.h"

/**
 * Quest System용 StateTree Schema
 * - Context로 QuestComponent와 QuestInstance를 자동 제공
 * - Quest 전용 Tasks/Conditions/Evaluators만 허용
 */
UCLASS(BlueprintType, EditInlineNew, CollapseCategories, meta = (DisplayName = "Quest StateTree Schema"))
class KORAPROJECT_API UQuestStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

public:
	UQuestStateTreeSchema();

	// Context 데이터 정의 (QuestComponent, QuestInstance 등)
	//virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;

	virtual bool Link(FStateTreeLinker& Linker) override;

protected:
	// Quest 전용 노드만 허용하도록 필터링
	virtual bool IsStructAllowed(const UScriptStruct* InStruct) const override;
	virtual bool IsClassAllowed(const UClass* InClass) const override;
	virtual bool IsExternalItemAllowed(const UStruct& InStruct) const override;
};
