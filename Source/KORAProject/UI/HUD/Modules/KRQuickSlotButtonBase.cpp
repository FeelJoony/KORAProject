// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRQuickSlotButtonBase.h"
#include "UI/HUD/Modules/KRQuickSlotWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UKRQuickSlotButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (UKRQuickSlotWidget* Parent = GetTypedOuter<UKRQuickSlotWidget>())
	{
		Parent->NotifySlotHovered(SlotDirection);
	}
}

void UKRQuickSlotButtonBase::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	Super::NativeDestruct();
}

void UKRQuickSlotButtonBase::PlayItemDurationTimer(float InDuration)
{
    if (InDuration <= 0.f || !DurationTimer) return;
    UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().ClearTimer(DurationTimerHandle);

    DurationTotalTime = InDuration;
    DurationElapsedTime = 0.f;

    if (!DurationMID)
    {
        DurationMID = DurationTimer->GetDynamicMaterial();
    }

    if (DurationMID)
    {
        DurationMID->SetScalarParameterValue(DurationParameterName, 1.0f);
    }

    World->GetTimerManager().SetTimer(
        DurationTimerHandle,
        this,
        &UKRQuickSlotButtonBase::UpdateDurationTimer,
        DurationUpdateInterval,
        true
    );
}

void UKRQuickSlotButtonBase::UpdateDurationTimer()
{
    UWorld* World = GetWorld();
    if (!World || DurationTotalTime <= 0.f)
    {
        if (World)
        {
            World->GetTimerManager().ClearTimer(DurationTimerHandle);
        }
        return;
    }

    DurationElapsedTime += DurationUpdateInterval;
    float Alpha = FMath::Clamp(DurationElapsedTime / DurationTotalTime, 0.f, 1.f);
    float Percent = 1.f - Alpha;

    if (DurationMID) DurationMID->SetScalarParameterValue(DurationParameterName, Percent);
    if (Alpha >= 1.f) World->GetTimerManager().ClearTimer(DurationTimerHandle);
}
