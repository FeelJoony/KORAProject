#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_SetRageStatus.generated.h"

UCLASS()
class KORAPROJECT_API UKRAIStateTree_SetRageStatus : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	bool bSetRage = false;
};
