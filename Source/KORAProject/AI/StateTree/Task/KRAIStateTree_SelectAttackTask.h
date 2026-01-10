#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_SelectAttackTask.generated.h"

USTRUCT(BlueprintType)
struct FAbilitySelectableCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	bool bIsSelectable = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	bool bInvert = false;

	bool ConditionResult() const
	{
		return bInvert ? !bIsSelectable : bIsSelectable;
	}
};

USTRUCT(BlueprintType)
struct FSelectAttackAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	float AcceptableAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	FAbilitySelectableCondition SelectCondition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter")
	int32 Weight = 0;
};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	TArray<FSelectAttackAbilityInfo> AbilityInfos;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	// int
};
