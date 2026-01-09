#include "KRAIStateTree_RunEQSQuery.h"

#include "StateTreeExecutionContext.h"
#include "AI/KRAIEnemyController.h"
#include "Characters/KRHeroCharacter.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UKRAIStateTree_RunEQSQuery::UKRAIStateTree_RunEQSQuery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bFinished = false;
}

EStateTreeRunStatus UKRAIStateTree_RunEQSQuery::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	// if (AIController->TargetActor)
	// {
	// 	return EStateTreeRunStatus::Succeeded;
	// }
	
	FEnvQueryRequest Request(EnvQuery, AIController);

	for (FAIDynamicParam& DynamicParam : QueryConfig)
	{
		Request.SetDynamicParam(DynamicParam, nullptr);
	}
	
	RequestId = Request.Execute(RunMode,
		FQueryFinishedSignature::CreateLambda([WeakContext = Context.MakeWeakExecutionContext(), this](TSharedPtr<FEnvQueryResult> QueryResult) mutable
		{
			const FStateTreeStrongExecutionContext StrongContext = WeakContext.MakeStrongExecutionContext();
			RequestId = INDEX_NONE;

			bool bSuccess = false;
			if (QueryResult && QueryResult->IsSuccessful())
			{
				TArray<AActor*> TargetActors;
				QueryResult->GetAllAsActors(TargetActors);
				for (AActor* TargetActor : TargetActors)
				{
					if (AKRHeroCharacter* Target = Cast<AKRHeroCharacter>(TargetActor))
					{
						AIController->TargetActor = Target;

						break;
					}
				}

				bSuccess = true;
			}

			StrongContext.FinishTask(bSuccess ? EStateTreeFinishTaskType::Succeeded : EStateTreeFinishTaskType::Failed);
			
		}));

	return RequestId != INDEX_NONE ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}

void UKRAIStateTree_RunEQSQuery::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	if (RequestId != INDEX_NONE)
	{
		if (UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(Context.GetOwner()))
		{
			QueryManager->AbortQuery(RequestId);
		}
		RequestId = INDEX_NONE;
	}
	
	Super::ExitState(Context, Transition);
}


