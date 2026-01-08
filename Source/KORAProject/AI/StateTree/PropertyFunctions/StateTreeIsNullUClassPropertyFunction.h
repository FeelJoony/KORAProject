#pragma once

#include "StateTreePropertyFunctionBase.h"

#include "StateTreeIsNullUClassPropertyFunction.generated.h"

USTRUCT()
struct FStateTreeIsNullUClassPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	TSubclassOf<UClass> Input = nullptr;

	UPROPERTY(EditAnywhere, Category = Output)
	bool Output = false;
};

USTRUCT(DisplayName = "Is Null UClass")
struct KORAPROJECT_API FStateTreeIsNullUClassPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeIsNullUClassPropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const;
#endif
};
