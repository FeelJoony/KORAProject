#include "KRAnimNotify_WorldEvent.h"
#include "Components/KRWorldEventComponent.h"

void UKRAnimNotify_WorldEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner() || !WorldEventTag.IsValid())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	UKRWorldEventComponent* WorldEventComp = Owner->FindComponentByClass<UKRWorldEventComponent>();
	if (WorldEventComp)
	{
		WorldEventComp->TriggerWorldEvent(WorldEventTag, Owner);
	}
}

#if WITH_EDITOR
FString UKRAnimNotify_WorldEvent::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("WorldEvent: %s"), *WorldEventTag.ToString());
}
#endif
