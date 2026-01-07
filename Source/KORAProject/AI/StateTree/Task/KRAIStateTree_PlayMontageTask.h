#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "KRAIStateTree_PlayMontageTask.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "KRAI",
	meta = (DisplayName = "Play Montage",
		Description = "A task that plays an animation montage on the context actor.",
		ToolTip = "Play an animation montage on the context actor.",
		Keywords = "Animation, Montage, Task, KRAI"))
class KORAPROJECT_API UKRAIStateTree_PlayMontageTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_PlayMontageTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task", meta = (RequiredAssetDataTags = "AnimMontage"))
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task", meta = (ClampMin = "0.01", ClampMax = "10.0"))
	float PlayRate = 1.f;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

private:
	bool bMontageFinished = false;
};
