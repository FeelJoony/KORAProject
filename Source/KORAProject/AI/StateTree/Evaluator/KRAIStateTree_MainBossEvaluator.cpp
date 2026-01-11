#include "KRAIStateTree_MainBossEvaluator.h"

#include "AI/KREnemyPawn.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

UKRAIStateTree_MainBossEvaluator::UKRAIStateTree_MainBossEvaluator(const FObjectInitializer& ObjectInitializer)
{
	
}

void UKRAIStateTree_MainBossEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	CurrentPhase = 0;

	if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
	{
		for (auto& AbilityInfoContainer : AbilityInfoContainers)
		{
			for (auto& AbilityInfo : AbilityInfoContainer.AbilityInfos)
			{
				FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityInfo.AbilityClass);
				if (Spec && Spec->Ability)
				{
					if (UGameplayAbility* Ability = Spec->Ability.Get())
					{
						Ability->OnGameplayAbilityEndedWithData.AddLambda([this](const FAbilityEndedData& EndedData)
						{
							SelectedAbilityClass = nullptr;
						});
					}
				}
			}
		}	
	}
}

void UKRAIStateTree_MainBossEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	for (int32 i = 0; i < AbilityInfoContainers.Num(); i++)
	{
		if (AbilityInfoContainers[i].EntryPageHealthPercent >= CurrentHealthPercent)
		{
			CurrentPhase = i;

			break;
		}
	}

	if (SelectedAbilityClass == nullptr)
	{
		int32 SelectedAbilityIndex = INDEX_NONE;
		EUseAbilityPriority SelectPriority = EUseAbilityPriority::Low;
		FMainBossEvaluatorAbilityInfoContainer& AbilityInfoContainer = AbilityInfoContainers[CurrentPhase];
		for (int i = AbilityInfoContainer.AbilityInfos.Num() - 1; i >= 0; i--)
		{
			FMainBossEvaluatorAbilityInfo& AbilityInfo = AbilityInfoContainer.AbilityInfos[i];
			if (AbilityInfo.CanExecute())
			{
				if (AbilityInfo.Priority > SelectPriority)
				{
					SelectedAbilityIndex = i;
					SelectPriority = AbilityInfo.Priority;

					break;
				}
			}
			else
			{
				AbilityInfo.CooldownLeft = FMath::Max(0.f, AbilityInfo.CooldownLeft - DeltaTime);
			}
		}

		if (SelectedAbilityIndex != INDEX_NONE)
		{
			AbilityInfoContainer.AbilityInfos[SelectedAbilityIndex].CooldownLeft = AbilityInfoContainer.AbilityInfos[SelectedAbilityIndex].Cooldown;
			SelectedAbilityClass = AbilityInfoContainer.AbilityInfos[SelectedAbilityIndex].AbilityClass;
		}	
	}
}

void UKRAIStateTree_MainBossEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}
