#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeEvaluatorBase.h"
#include "KRAIStateTree_PlayerEvaluator.generated.h"

UCLASS(Blueprintable,
	BlueprintType,
	meta = (DisplayName = "Player Evaluator",
		category = "KRAI",
		Description = "A state tree evaluator that checks if the context actor is a player.",
		ToolTip = "A state tree evaluator that checks if the context actor is a player.",
		Keywords = "Player, Evaluator, Task, AI, KRAI",
		Icon = "Check_Circle"))
class KORAPROJECT_API UKRAIStateTree_PlayerEvaluator : public UKRAIStateTreeEvaluatorBase
{
	GENERATED_BODY()

public:
	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context)
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context)
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	TObjectPtr<class AActor> PlayerActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	float DistanceToPlayer = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	bool bIsPlayerAttacking = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	bool bIsPlayerDead = false;

private:
	void SetDefaultValues();
};
