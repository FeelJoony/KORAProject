// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "UI/HUD/KRHUDWidgetInterface.h"
#include "KRHUDWidgetBase.generated.h"

class UAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API UKRHUDWidgetBase : public UCommonUserWidget, public IKRHUDWidgetInterface
{
	GENERATED_BODY()
	
public:
	virtual void InitializeHUD(const FKRHUDInitArgs& Args) override;
	virtual void TearDownHUD() override;

protected:
	virtual void OnHUDInitialized() {}
	virtual void UnbindAll() {}

	TWeakObjectPtr<UAbilitySystemComponent> ASCWeak;
	//TWeakObjectPtr<AActor> AvatarWeak;
};
