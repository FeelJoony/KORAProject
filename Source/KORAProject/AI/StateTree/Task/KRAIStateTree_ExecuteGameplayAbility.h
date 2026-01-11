#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_ExecuteGameplayAbility.generated.h"

UCLASS()
class KORAPROJECT_API UKRAIStateTree_ExecuteGameplayAbility : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_ExecuteGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayAbility> AbilityClass;
};
