#include "StateTreeIsNullObjectPropertyFunction.h"

#include "StateTreeExecutionContext.h"

#define LOCTEXT_NAMESPACE "StateTree"

void FStateTreeIsNullObjectPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.Output = InstanceData.Input == nullptr;
}

#if WITH_EDITOR
FText FStateTreeIsNullObjectPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FStateTreePropertyFunctionCommonBase::GetDescription(ID, InstanceDataView, BindingLookup, Formatting);
}

#endif

#undef LOCTEXT_NAMESPACE
