#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "KRAIStateTree_OverlapPlayerEvaluator.generated.h"

UCLASS(Category = "KRAI", meta = (DisplayName = "Overlap Player", ToolTip = "A task that sends an event to the StateTree.", Keywords = "Event, Task, AI, KRAI"))
class KORAPROJECT_API UKRAIStateTree_OverlapPlayerEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_OverlapPlayerEvaluator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TreeStart(FStateTreeExecutionContext& Context) override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	virtual void TreeStop(FStateTreeExecutionContext& Context) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AKREnemyPawn> EnemyCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<class AKRAIEnemyController> AIController;

	float OverlapRadius = 0.f;

private:
	struct FEnemyDataStruct* EnemyData = nullptr;
	struct FEnemyAttributeDataStruct* EnemyAttributeData = nullptr;
	
	
	float LastCheckOverlapTime = 0.f;
	const float OverlapCheckInterval = 0.5f;
};
