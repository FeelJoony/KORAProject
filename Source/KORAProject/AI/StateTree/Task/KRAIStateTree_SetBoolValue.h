#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_SetBoolValue.generated.h"

UCLASS()
class KORAPROJECT_API UKRAIStateTree_SetBoolValue : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_SetBoolValue(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (Input))
	bool bSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	bool bValue;
};
