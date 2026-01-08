#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_SelectAttackTask.generated.h"

UCLASS(Category = "KRAI", meta = (DisplayName = "Select Attack Ability", ToolTip = "", Keywords = "Select, Attack, Task, AI, KRAI"))
class KORAPROJECT_API UKRAIStateTree_SelectAttackTask : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_SelectAttackTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Context)
	TObjectPtr<class AKREnemyPawn> Actor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	float AcceptableAttackRange;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	// int
};
