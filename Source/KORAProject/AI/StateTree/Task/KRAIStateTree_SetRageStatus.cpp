// Fill out your copyright notice in the Description page of Project Settings.


#include "KRAIStateTree_SetRageStatus.h"

#include "AI/KREnemyPawn.h"

EStateTreeRunStatus UKRAIStateTree_SetRageStatus::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (Actor)
	{
		Actor->SetRage(bSetRage);
	}
	
	return EStateTreeRunStatus::Succeeded;
}
