#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "KRStateTreeTask_Alert.generated.h"

UCLASS()
class KORAPROJECT_API UKRStateTreeTask_Alert : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
public:
	UKRStateTreeTask_Alert(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition);

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime);

	//virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition);

	void CallSenseRangeOnAIC(FStateTreeExecutionContext& Context);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AlertTime")
	float AttackDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AlertTime")
	float ElapsedTime;
};
