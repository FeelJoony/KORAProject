// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRQuickSlotButtonBase.h"
#include "UI/HUD/Modules/KRQuickSlotWidget.h"

void UKRQuickSlotButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (UKRQuickSlotWidget* Parent = GetTypedOuter<UKRQuickSlotWidget>())
	{
		Parent->NotifySlotHovered(SlotDirection);
	}
}
