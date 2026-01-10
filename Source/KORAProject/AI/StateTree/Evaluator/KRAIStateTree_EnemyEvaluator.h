#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeEvaluatorBase.h"
#include "KRAIStateTree_EnemyEvaluator.generated.h"

USTRUCT(BlueprintType)
struct FEnemyEvaluatorInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output)
	TObjectPtr<AActor> TargetActor = nullptr;
};

UCLASS(Blueprintable,
	BlueprintType,
	meta = (DisplayName = "EnemyEvaluator",
		category = "KRAI",
		Description = "A state tree evaluator that checks if the context actor is a player.",
		ToolTip = "A evaluator that check result",
		Keywords = "Enemy, Evaluator, Task, AI, KRAI",
		Icon = "Check_Circle"))
class KORAPROJECT_API UKRAIStateTree_EnemyEvaluator : public UKRAIStateTreeEvaluatorBase
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	bool bCanAttackRange = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	bool bIsEnemyDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	bool bIsRageStatus = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	float CanAttackRange = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	float DistanceToTarget = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	float EnterRageStatusRate = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output)
	float CurrentHealthPercent = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
	float EnterRageHealthPercent = 0.f;
	
private:
	void SetDefaultValues();
};
