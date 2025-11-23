// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KRHUDWidgetBase.h"
#include "AbilitySystemComponent.h"

void UKRHUDWidgetBase::InitializeHUD_Implementation(const FKRHUDInitArgs& Args)
{
    ASCWeak = Args.ASC;
    OnHUDInitialized();
}

void UKRHUDWidgetBase::TearDownHUD_Implementation()
{
    UnbindAll();
    ASCWeak = nullptr;
}

void UKRHUDWidgetBase::NativeDestruct()
{
    TearDownHUD_Implementation();
    Super::NativeDestruct();
}