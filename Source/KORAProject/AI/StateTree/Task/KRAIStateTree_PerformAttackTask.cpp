#include "KRAIStateTree_PerformAttackTask.h"

#include "AbilitySystemComponent.h"
#include "AI/KREnemyPawn.h"
#include "Abilities/GameplayAbility.h"

EStateTreeRunStatus UKRAIStateTree_PerformAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (Actor && AbilityClass != nullptr)
	{
		if (!Actor->TryActivateAbility(AbilityClass))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to activate ability"));
		
			return EStateTreeRunStatus::Failed;
		}

		// if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
		// {
		// 	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);
		// 	if (Spec && Spec->Ability)
		// 	{
		// 		bEndedAbility = false;
		// 		Spec->Ability->OnGameplayAbilityEndedWithData.AddUObject(this, &UKRAIStateTree_PerformAttackTask::OnGameplayAbilityEndedCallback);
		// 		if (!ASC->TryActivateAbility(Spec->Handle))
		// 		{
		// 			UE_LOG(LogTemp, Error, TEXT("Failed to activate ability"));
		//
		// 			return EStateTreeRunStatus::Failed;
		// 		}
		// 	}
		// }
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UKRAIStateTree_PerformAttackTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
	{
		if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass))
		{
			return Spec->IsActive() ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
		}
	}

	// if (bEndedAbility)
	// {
	// 	return EStateTreeRunStatus::Succeeded;
	// }
	
	return Super::Tick(Context, DeltaTime);
}

void UKRAIStateTree_PerformAttackTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	bEndedAbility = false;
	
	Actor->OnUnselectAttack.Broadcast();
	
	Super::ExitState(Context, Transition);
}

void UKRAIStateTree_PerformAttackTask::OnGameplayAbilityEndedCallback(const FAbilityEndedData& EndedData)
{
	bEndedAbility = true;
}
