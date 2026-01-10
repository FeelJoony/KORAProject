#include "KRAIStateTree_PlayGameplayAbilityTask.h"

#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"
#include "AI/KREnemyPawn.h"

UKRAIStateTree_PlayGameplayAbilityTask::UKRAIStateTree_PlayGameplayAbilityTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivateSpec = nullptr;
}

EStateTreeRunStatus UKRAIStateTree_PlayGameplayAbilityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
	{
		if (ASC->TryActivateAbilitiesByTag(AbilityTags))
		{
			TArray<FGameplayAbilitySpecHandle> FindAbilities;
			ASC->FindAllAbilitiesWithTags(FindAbilities, AbilityTags);
			if (FindAbilities.Num() > 0)
			{
				for (FGameplayAbilitySpecHandle& FindAbility : FindAbilities)
				{
					ActivateSpec = ASC->FindAbilitySpecFromHandle(FindAbility);
				}
			}
		}
	}
	
	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus UKRAIStateTree_PlayGameplayAbilityTask::Tick(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	if (ActivateSpec && ActivateSpec->IsActive())
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Succeeded;
}

void UKRAIStateTree_PlayGameplayAbilityTask::EndState(FStateTreeExecutionContext& Context,
	FStateTreeTransitionResult& OutState)
{
	ActivateSpec = nullptr;
}

