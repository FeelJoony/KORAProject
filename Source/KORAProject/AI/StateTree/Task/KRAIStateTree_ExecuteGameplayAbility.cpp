#include "KRAIStateTree_ExecuteGameplayAbility.h"

#include "AI/KREnemyPawn.h"
#include "AbilitySystemComponent.h"

UKRAIStateTree_ExecuteGameplayAbility::UKRAIStateTree_ExecuteGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

EStateTreeRunStatus UKRAIStateTree_ExecuteGameplayAbility::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
	{
		if (!ASC->TryActivateAbilityByClass(AbilityClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to activate ability %s"), *AbilityClass->GetName());
			
			return EStateTreeRunStatus::Failed;
		}
	}
	
	return EStateTreeRunStatus::Succeeded;
}

void UKRAIStateTree_ExecuteGameplayAbility::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}
