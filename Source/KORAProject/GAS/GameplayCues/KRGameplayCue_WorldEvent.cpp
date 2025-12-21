#include "KRGameplayCue_WorldEvent.h"
#include "Components/KRWorldEventComponent.h"

bool UKRGameplayCue_WorldEvent::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!MyTarget || !WorldEventData.Get())
	{
		return false;
	}

	UKRWorldEventComponent* WorldEventComp = MyTarget->FindComponentByClass<UKRWorldEventComponent>();
	if (WorldEventComp)
	{
		UKRWorldEventData* Data = WorldEventData.LoadSynchronous();
		if (Data)
		{
			WorldEventComp->TriggerWorldEventFromData(Data, MyTarget);
			return true;
		}
	}

	return false;
}
