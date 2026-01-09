#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "KRAIStateTree_RunEQSQuery.generated.h"

UCLASS()
class KORAPROJECT_API UKRAIStateTree_RunEQSQuery : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_RunEQSQuery(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	TObjectPtr<class UEnvQuery> EnvQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	TArray<FAIDynamicParam> QueryConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;

private:
	bool bFinished = false;

	int32 RequestId = INDEX_NONE;
};
