#pragma once

#include "StateTreePropertyFunctionBase.h"
#include "GameplayTagContainer.h"

#include "StateTreeEqualPayloadTagValuePropertyFunction.generated.h"

USTRUCT()
struct FStateTreeEqualPayloadTagValuePropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag LHand = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag RHand = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, Category = Output)
	bool Output = false;
};

USTRUCT(DisplayName = "Equal Payload Tag Value")
struct KORAPROJECT_API FStateTreeEqualPayloadTagValuePropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeEqualPayloadTagValuePropertyFunctionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const;
#endif
};
