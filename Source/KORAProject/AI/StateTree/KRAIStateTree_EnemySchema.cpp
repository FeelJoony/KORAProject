#include "KRAIStateTree_EnemySchema.h"
#include "AI/KREnemyPawn.h"
#include "AI/KRAIEnemyController.h"
#include "Evaluator/KRAIStateTreeEvaluatorBase.h"
#include "Task/KRAIStateTreeTaskBase.h"
#include "Condition/KRAIStateTreeConditionBase.h"

TArray<FStateTreeExternalDataDesc> UKRAIStateTree_EnemySchema::ContextDataDescs;

UKRAIStateTree_EnemySchema::UKRAIStateTree_EnemySchema()
{
	ContextActorClass = AKREnemyPawn::StaticClass();
	AIControllerClass = AKRAIEnemyController::StaticClass();

	if (ContextDataDescs.IsEmpty())
	{
		ContextDataDescs.Add(FStateTreeExternalDataDesc(
			FName(TEXT("Actor")),
			AKREnemyPawn::StaticClass(),
			FGuid::NewGuid()
		));

		ContextDataDescs.Add(FStateTreeExternalDataDesc(
			FName(TEXT("AIController")),
			AKRAIEnemyController::StaticClass(),
			FGuid::NewGuid()
		));
	}
}

TConstArrayView<FStateTreeExternalDataDesc> UKRAIStateTree_EnemySchema::GetContextDataDescs() const
{
	return ContextDataDescs;
}

bool UKRAIStateTree_EnemySchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return true;
}

bool UKRAIStateTree_EnemySchema::IsClassAllowed(const UClass* InScriptStruct) const
{
	if (InScriptStruct->IsChildOf(UKRAIStateTreeTaskBase::StaticClass())
	 || InScriptStruct->IsChildOf(UKRAIStateTreeConditionBase::StaticClass())
	 || InScriptStruct->IsChildOf(UKRAIStateTreeEvaluatorBase::StaticClass()))
	{
		return true;
	}

	return Super::IsClassAllowed(InScriptStruct);
}
