#include "KRAIStateTree_SpinderBreedEvaluator.h"

#include "AbilitySystemComponent.h"
#include "AI/KREnemyPawn.h"
#include "Abilities/GameplayAbility.h"

void UKRAIStateTree_SpinderBreedEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
	
	bHasExecuteSpawnSkill = false;
}

void UKRAIStateTree_SpinderBreedEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (!bHasExecuteSpawnSkill)
	{
		if (Actor && SpawnSkillAbilityClass)
		{
			if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
			{
				if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(SpawnSkillAbilityClass))
				{
					if (Spec->IsActive())
					{
						bHasExecuteSpawnSkill = true;				
					}
				}
			}
		}
	}
}

void UKRAIStateTree_SpinderBreedEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	bHasExecuteSpawnSkill = false;
	
	Super::TreeStop(Context);
}
