#include "KRAIStateTree_AttackEvaluator.h"
#include "AI/KREnemyPawn.h"
#include "GAS/Abilities/KRGameplayAbility.h"

UKRAIStateTree_AttackEvaluator::UKRAIStateTree_AttackEvaluator(const FObjectInitializer& ObjectInitializer)
{
}

void UKRAIStateTree_AttackEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	if (Actor)
	{
		OnSelectDelegateHandle = Actor->OnSelectAttack.AddUObject(this, &ThisClass::OnSelectAttack);
		OnUnselectDelegateHandle = Actor->OnUnselectAttack.AddUObject(this, &ThisClass::OnUnselectAttack);
	}
}

void UKRAIStateTree_AttackEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void UKRAIStateTree_AttackEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	Actor->OnSelectAttack.Remove(OnSelectDelegateHandle);
	Actor->OnUnselectAttack.Remove(OnUnselectDelegateHandle);

	AttackAbilityClass = nullptr;
	AcceptableAttackRange = 0.f;
	
	Super::TreeStop(Context);
}

void UKRAIStateTree_AttackEvaluator::OnSelectAttack(TSubclassOf<UGameplayAbility> InAttackAbility, float InAcceptableAttackRange)
{
	if (AttackAbilityClass != nullptr)
	{
		return;
	}

	AttackAbilityClass = InAttackAbility;
	AcceptableAttackRange = InAcceptableAttackRange;
}

void UKRAIStateTree_AttackEvaluator::OnUnselectAttack()
{
	AttackAbilityClass = nullptr;
	AcceptableAttackRange = 0.f;
}
