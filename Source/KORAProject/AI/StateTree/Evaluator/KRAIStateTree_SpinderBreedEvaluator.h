#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeEvaluatorBase.h"
#include "KRAIStateTree_SpinderBreedEvaluator.generated.h"

UCLASS()
class KORAPROJECT_API UKRAIStateTree_SpinderBreedEvaluator : public UKRAIStateTreeEvaluatorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	TSubclassOf<class UGameplayAbility> SpawnSkillAbilityClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output, meta = (Output))
	bool bHasExecuteSpawnSkill = false;

protected:
	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) override;
};
