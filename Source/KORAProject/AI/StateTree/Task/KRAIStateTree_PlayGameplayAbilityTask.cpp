#include "KRAIStateTree_PlayGameplayAbilityTask.h"

#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"
#include "AI/KREnemyPawn.h"

UKRAIStateTree_PlayGameplayAbilityTask::UKRAIStateTree_PlayGameplayAbilityTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivateSpec = nullptr;
	ActivateType = EKRAI_PlayGameplayAbilityTaskActivateType::SingleUse; 
}

EStateTreeRunStatus UKRAIStateTree_PlayGameplayAbilityTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
	{
		ActivateSpec = ASC->FindAbilitySpecFromClass(AbilityClass);
		if (!ActivateSpec || !ASC->TryActivateAbility(ActivateSpec->Handle))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to activate ability"));
			
			return EStateTreeRunStatus::Failed;
		}
		
		// if (ASC->TryActivateAbilitiesByTag(AbilityTags))
		// {
		// 	TArray<FGameplayAbilitySpecHandle> FindAbilities;
		// 	ASC->FindAllAbilitiesWithTags(FindAbilities, AbilityTags);
		// 	if (FindAbilities.Num() > 0)
		// 	{
		// 		for (FGameplayAbilitySpecHandle& FindAbility : FindAbilities)
		// 		{
		// 			ActivateSpec = ASC->FindAbilitySpecFromHandle(FindAbility);
		// 		}
		// 	}
		// }

		if (ActivateType == EKRAI_PlayGameplayAbilityTaskActivateType::SingleUse)
		{
			ActivateSpec->Ability->K2_CancelAbility();

			FinishTask();
			
			return EStateTreeRunStatus::Succeeded;
		}
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UKRAIStateTree_PlayGameplayAbilityTask::Tick(FStateTreeExecutionContext& Context,
	const float DeltaTime)
{
	if (ActivateType == EKRAI_PlayGameplayAbilityTaskActivateType::SingleUse)
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	if (ActivateSpec && ActivateSpec->IsActive())
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Succeeded;
}

void UKRAIStateTree_PlayGameplayAbilityTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	ActivateSpec = nullptr;
	
	Super::ExitState(Context, Transition);
}

