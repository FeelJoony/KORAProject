#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponentSchema.h"
#include "KRAIStateTree_EnemySchema.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "KR Enemy AI State Tree Schema"))
class KORAPROJECT_API UKRAIStateTree_EnemySchema : public UStateTreeAIComponentSchema
{
	GENERATED_BODY()

public:
	UKRAIStateTree_EnemySchema();

	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;

	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

	virtual bool IsClassAllowed(const UClass* InScriptStruct) const override;

private:
	static TArray<FStateTreeExternalDataDesc> ContextDataDescs;
};
