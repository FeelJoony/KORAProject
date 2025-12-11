#include "StateTree/KRStateTreeTask_Alert.h"
#include "Characters/KREnemyCharacter.h"
#include "Enemy/KRAIC_Enemy.h"
#include "Interface/AICInterface.h"

#include "StateTreeExecutionContext.h"


UKRStateTreeTask_Alert::UKRStateTreeTask_Alert(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bShouldCallTick = true;

	AttackDelayTime = 1.5f;

	ElapsedTime = 0.0f;
}

//EStateTreeRunStatus UKRStateTreeTask_Alert::EnterState(FStateTreeExecutionContext& Context,
//										const FStateTreeTransitionResult& Transition)
//{
//	return EStateTreeRunStatus::Running;
//}

EStateTreeRunStatus UKRStateTreeTask_Alert::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	ElapsedTime += DeltaTime;
	if (ElapsedTime <= AttackDelayTime)
	{
		return EStateTreeRunStatus::Running;
	}

	CallSenseRangeOnAIC(Context);

	return EStateTreeRunStatus::Succeeded;
}

//void UKRStateTreeTask_Alert::ExitState(FStateTreeExecutionContext& Context,
//										const FStateTreeTransitionResult& Transition)
//{
//
//}

void UKRStateTreeTask_Alert::CallSenseRangeOnAIC(FStateTreeExecutionContext& Context)
{
	AAIController* AIC = Cast<AAIController>(Context.GetOwner());
	if (!IsValid(AIC)) return;

	AKRAIC_Enemy* AIC_Enemy = Cast<AKRAIC_Enemy>(AIC);
	if (!IsValid(AIC_Enemy)) return;
	
	if (AIC_Enemy->GetClass()->ImplementsInterface(UAICInterface::StaticClass()))
	{
		AIC_Enemy->SenseRange();
	}
}