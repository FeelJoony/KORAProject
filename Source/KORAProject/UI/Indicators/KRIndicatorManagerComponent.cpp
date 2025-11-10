#include "KRIndicatorManagerComponent.h"
#include "IndicatorDescriptor.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

UKRIndicatorManagerComponent::UKRIndicatorManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UKRIndicatorManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UKRIndicatorManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateIndicators(DeltaTime);
}

void UKRIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (!IndicatorDescriptor)
	{
		return;
	}

	// Don't add duplicates
	if (Indicators.Contains(IndicatorDescriptor))
	{
		return;
	}

	Indicators.Add(IndicatorDescriptor);

	// Create the widget if needed
	if (IndicatorDescriptor->IndicatorWidgetClass && !IndicatorDescriptor->IndicatorWidget)
	{
		if (APlayerController* PC = GetOwningPlayerController())
		{
			IndicatorDescriptor->IndicatorWidget = CreateWidget<UUserWidget>(PC, IndicatorDescriptor->IndicatorWidgetClass);
			if (IndicatorDescriptor->IndicatorWidget)
			{
				IndicatorDescriptor->IndicatorWidget->AddToViewport();
			}
		}
	}
}

void UKRIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (!IndicatorDescriptor)
	{
		return;
	}

	Indicators.Remove(IndicatorDescriptor);

	// Clean up the widget
	if (IndicatorDescriptor->IndicatorWidget)
	{
		IndicatorDescriptor->IndicatorWidget->RemoveFromParent();
		IndicatorDescriptor->IndicatorWidget = nullptr;
	}
}

void UKRIndicatorManagerComponent::UpdateIndicators(float DeltaTime)
{
	// Clean up any invalid indicators
	Indicators.RemoveAll([](UIndicatorDescriptor* Indicator)
	{
		return !IsValid(Indicator);
	});

	// Update each indicator
	for (UIndicatorDescriptor* Indicator : Indicators)
	{
		if (!Indicator || !Indicator->IndicatorWidget)
		{
			continue;
		}

		// Update visibility
		if (Indicator->bVisible)
		{
			Indicator->IndicatorWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Indicator->IndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		// Here you would update the position of the indicator based on the world location
		// This requires projecting world space to screen space
		// For now, we just ensure the widget is visible/hidden
	}
}

APlayerController* UKRIndicatorManagerComponent::GetOwningPlayerController() const
{
	if (AActor* Owner = GetOwner())
	{
		return Cast<APlayerController>(Owner);
	}
	return nullptr;
}
