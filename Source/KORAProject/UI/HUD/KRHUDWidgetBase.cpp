// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/KRHUDWidgetBase.h"

void UKRHUDWidgetBase::InitializeHUD(const FKRHUDInitArgs& Args)
{
	//ASCWeak = Args.ASC;
	//AvatarWeak = Args.Avatar;
	OnHUDInitialized();
}

void UKRHUDWidgetBase::TearDownHUD()
{
	UnbindAll();
	//ASCWeak.Reset();
	//AvatarWeak.Reset();
}
