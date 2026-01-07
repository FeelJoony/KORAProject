#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_PlayGameplayAbilityTask.generated.h"

UCLASS(Category = "KRAI", meta = (DisplayName = "Play Enemy GameplayAbility", ToolTip = "A task that play enemy GameplayAbility", Keywords = "Ability, Task, AI, KRAI"))
class KORAPROJECT_API UKRAIStateTree_PlayGameplayAbilityTask : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_PlayGameplayAbilityTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter", meta = (AllowPrivateAccess = "true"))
	FGameplayTag AbilityTag;
};
