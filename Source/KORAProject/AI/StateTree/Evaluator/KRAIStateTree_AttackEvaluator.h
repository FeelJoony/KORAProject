#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeEvaluatorBase.h"
#include "KRAIStateTree_AttackEvaluator.generated.h"

UCLASS(Blueprintable,
	BlueprintType,
	meta = (DisplayName = "AttackEvaluator",
		category = "KRAI",
		Description = "A state tree evaluator that select and hold attack ability",
		Keywords = "Enemy, Evaluator, Task, AI, KRAI",
		Icon = "Check_Circle"))
class KORAPROJECT_API UKRAIStateTree_AttackEvaluator : public UKRAIStateTreeEvaluatorBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_AttackEvaluator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Context, meta = (Context, AllowPrivateAccess = "true"))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayAbility> AttackAbilityClass = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
	float AcceptableAttackRange = 0.f;

	void OnSelectAttack(TSubclassOf<class UGameplayAbility> InAttackAbility, float InAcceptableAttackRange);

	void OnUnselectAttack();

private:
	FDelegateHandle OnSelectDelegateHandle;
	FDelegateHandle OnUnselectDelegateHandle;
};
