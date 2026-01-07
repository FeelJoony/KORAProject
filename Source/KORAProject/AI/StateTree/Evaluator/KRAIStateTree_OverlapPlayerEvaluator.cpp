#include "KRAIStateTree_OverlapPlayerEvaluator.h"

#include "StateTreeExecutionContext.h"
#include "AI/KREnemyPawn.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/EnemyDataStruct.h"
#include "Data/EnemyAbilityDataStruct.h"
#include "Data/EnemyAttributeDataStruct.h"
#include "GameplayTag/KREnemyTag.h"

UKRAIStateTree_OverlapPlayerEvaluator::UKRAIStateTree_OverlapPlayerEvaluator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRAIStateTree_OverlapPlayerEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	const FGameplayTag& EnemyTag = EnemyCharacter->GetEnemyTag();

	UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(this);
	EnemyData = DataTablesSubsystem.GetData<FEnemyDataStruct>(EnemyTag);
	EnemyAbilityData = DataTablesSubsystem.GetData<FEnemyAbilityDataStruct>(EnemyTag);
	EnemyAttributeData = DataTablesSubsystem.GetData<FEnemyAttributeDataStruct>(EnemyTag);

	const FStateTreeEventQueue& EventQueue = Context.GetEventQueue();
	for (const FStateTreeSharedEvent& Event : EventQueue.GetEventsView())
	{
		if (Event->Tag == KRTAG_ENEMY_AISTATE_DEAD)
		{
			
		}
	}
	
	if (EnemyData)
	{
		
	}
	
	LastCheckOverlapTime = 0.f;
}

void UKRAIStateTree_OverlapPlayerEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	LastCheckOverlapTime += DeltaTime;
	if (OverlapCheckInterval <= LastCheckOverlapTime)
	{
		
		
		LastCheckOverlapTime = 0.f;
	}
}

void UKRAIStateTree_OverlapPlayerEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
	Super::TreeStop(Context);
}
