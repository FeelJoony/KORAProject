#include "IndicatorDescriptor.h"
#include "Components/SceneComponent.h"

UIndicatorDescriptor::UIndicatorDescriptor()
	: DataObject(nullptr)
	, ComponentToTrack(nullptr)
	, bVisible(true)
	, bClampToScreen(false)
	, Priority(0)
	, IndicatorWidgetClass(nullptr)
	, IndicatorWidget(nullptr)
{
}

FVector UIndicatorDescriptor::GetIndicatorLocation() const
{
	if (ComponentToTrack.IsValid())
	{
		return ComponentToTrack->GetComponentLocation();
	}

	if (DataObject)
	{
		if (AActor* Actor = Cast<AActor>(DataObject))
		{
			return Actor->GetActorLocation();
		}
	}

	return FVector::ZeroVector;
}

void UIndicatorDescriptor::SetSceneComponent(USceneComponent* Component)
{
	ComponentToTrack = Component;
}

void UIndicatorDescriptor::SetDataObject(UObject* InDataObject)
{
	DataObject = InDataObject;
}
