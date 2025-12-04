#include "KRAbilityTask_WaitTick.h"

UKRAbilityTask_WaitTick::UKRAbilityTask_WaitTick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UKRAbilityTask_WaitTick* UKRAbilityTask_WaitTick::WaitTick(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UKRAbilityTask_WaitTick* MyObj = NewAbilityTask<UKRAbilityTask_WaitTick>(OwningAbility, TaskInstanceName);
	return MyObj;
}

void UKRAbilityTask_WaitTick::Activate()
{
	Super::Activate();
}

void UKRAbilityTask_WaitTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
