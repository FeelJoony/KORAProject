#include "KRAIStateTree_EnemyEvaluator.h"

#include "AI/KREnemyPawn.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KREnemyAttributeSet.h"
#include "StateTreeExecutionContext.h"
#include "GameplayTag/KREnemyTag.h"
#include "AI/StateTree/KRAIStateTree_EventPayloadData.h"

void UKRAIStateTree_EnemyEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	SetDefaultValues();

	if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
	{
		if (const UKREnemyAttributeSet* EnemyAttributeSet = ASC->GetSet<UKREnemyAttributeSet>())
		{
			CanAttackRange = EnemyAttributeSet->GetCanAttackRange();
			EnterRageStatusRate = EnemyAttributeSet->GetEnterRageStatusRate();
		}
	}
}

void UKRAIStateTree_EnemyEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (bIsEnemyDead)
	{
		return;
	}

	if (Actor)
	{
		if (UKRAbilitySystemComponent* ASC = Cast<UKRAbilitySystemComponent>(Actor->GetAbilitySystemComponent()))
		{
			bool bFound = false;
			float CurrentHealth = ASC->GetGameplayAttributeValue(UKRCombatCommonSet::GetCurrentHealthAttribute(), bFound);
			float MaxHealth = ASC->GetGameplayAttributeValue(UKRCombatCommonSet::GetMaxHealthAttribute(), bFound);
			
			CurrentHealthPercent = FMath::Max(0.f, CurrentHealth / MaxHealth);

			bool bFoundRageStatus = false;
			EnterRageHealthPercent = ASC->GetGameplayAttributeValue(UKREnemyAttributeSet::GetEnterRageStatusRateAttribute(), bFoundRageStatus);
			bIsRageStatus = Actor->IsRage();
			
			if (bFound && CurrentHealth <= 0.f)
			{
				bIsEnemyDead = true;

				FKRAIStateTree_EventPayloadData Payload;
				Payload.ValueTag = Actor->GetEnemyTag();
				Context.SendEvent(KRTAG_ENEMY_AISTATE_DEAD, FConstStructView::Make(Payload));
			}
		}
	}
}

void UKRAIStateTree_EnemyEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}

void UKRAIStateTree_EnemyEvaluator::SetDefaultValues()
{
	bCanAttackRange = false;
	bIsEnemyDead = false;
	CanAttackRange = 0.f;
	DistanceToTarget = 0.f;
	EnterRageStatusRate = 0.f;
}
