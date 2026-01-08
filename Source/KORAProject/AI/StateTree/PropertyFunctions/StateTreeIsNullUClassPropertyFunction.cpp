#include "StateTreeIsNullUClassPropertyFunction.h"

#include "StateTreeExecutionContext.h"


void FStateTreeIsNullUClassPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType InstanceData = Context.GetInstanceData(*this);
	InstanceData.Output = InstanceData.Input == nullptr;
}

#if WITH_EDITOR
FText FStateTreeIsNullUClassPropertyFunction::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FStateTreePropertyFunctionCommonBase::GetDescription(ID, InstanceDataView, BindingLookup, Formatting);
}

#endif
