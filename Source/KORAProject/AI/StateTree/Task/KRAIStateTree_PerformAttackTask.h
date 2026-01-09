#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_PerformAttackTask.generated.h"

UENUM(BlueprintType, Category = "Attack",
	meta = (DisplayName = "Attack Selection Mode",
		Description = "Determines how the attack ability is selected.",
		Tooltip = "Determines how the attack ability is selected"))
enum class EKRAI_AttackSelectMode : uint8
{
	Sequential	UMETA(DisplayName = "Sequential"),
	Random		UMETA(DisplayName = "Random")
};

USTRUCT(BlueprintType, Category = "Attack",
	meta = (DisplayName = "Attack Ability Info",
		Description = "Contains information about an attack ability, including its tag and weight for selection.",
		Tooltip = "Contains information about an attack ability, including its tag and weight for selection."))
struct KORAPROJECT_API FKRAIStateTree_PerformAttackAbilityInfo
{
	GENERATED_BODY()

	
};

UCLASS(Category = "KRAI", meta = (DisplayName = "Perform Select Attack", ToolTip = "", Keywords = "Attack, Task, AI, KRAI"))
class KORAPROJECT_API UKRAIStateTree_PerformAttackTask : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TSubclassOf<class UGameplayAbility> AbilityClass;

private:
	bool bEndedAbility = false;

	void OnGameplayAbilityEndedCallback(const struct FAbilityEndedData& EndedData);
};
