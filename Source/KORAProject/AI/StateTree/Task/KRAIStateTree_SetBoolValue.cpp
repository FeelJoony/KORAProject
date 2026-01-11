#include "KRAIStateTree_SetBoolValue.h"

UKRAIStateTree_SetBoolValue::UKRAIStateTree_SetBoolValue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

EStateTreeRunStatus UKRAIStateTree_SetBoolValue::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	bSet = bValue;
	
	return EStateTreeRunStatus::Succeeded;
}
