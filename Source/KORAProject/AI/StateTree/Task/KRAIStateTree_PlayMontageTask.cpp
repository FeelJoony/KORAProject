#include "KRAIStateTree_PlayMontageTask.h"
#include "AI/KRAIEnemyController.h"
#include "AI/KREnemyPawn.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UKRAIStateTree_PlayMontageTask::UKRAIStateTree_PlayMontageTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShouldCallTick = true;
}

EStateTreeRunStatus UKRAIStateTree_PlayMontageTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!ensure(Actor && Montage))
	{
		return EStateTreeRunStatus::Failed;
	}

	if (USkeletalMeshComponent* MeshComp = Actor->GetComponentByClass<USkeletalMeshComponent>())
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			AnimInstance->Montage_Play(Montage, PlayRate);
		}
	}

	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus UKRAIStateTree_PlayMontageTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	return Super::Tick(Context, DeltaTime);
}

void UKRAIStateTree_PlayMontageTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}
