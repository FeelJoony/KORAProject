#pragma once

#include "StateTreePropertyFunctionBase.h"

#include "StateTreeIsNullObjectPropertyFunction.generated.h"

USTRUCT()
struct FStateTreeIsNullObjectPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	TObjectPtr<UObject> Input = nullptr;

	UPROPERTY(EditAnywhere, Category = Output)
	bool Output = false;
};

USTRUCT(DisplayName = "Is Null Object")
struct KORAPROJECT_API FStateTreeIsNullObjectPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeIsNullObjectPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const;
#endif
};
