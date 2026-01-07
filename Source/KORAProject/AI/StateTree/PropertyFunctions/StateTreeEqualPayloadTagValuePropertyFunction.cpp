#include "StateTreeEqualPayloadTagValuePropertyFunction.h"

#include "StateTreeExecutionContext.h"

void FStateTreeEqualPayloadTagValuePropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Output = InstanceData.LHand == InstanceData.RHand;
}

#if WITH_EDITOR
FText FStateTreeEqualPayloadTagValuePropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FStateTreePropertyFunctionCommonBase::GetDescription(ID, InstanceDataView, BindingLookup, Formatting);
}

#endif

#undef LOCTEXT_NAMESPACE
