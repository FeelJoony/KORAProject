// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "KRHUDWidgetInterface.generated.h"

class UAbilitySystemComponent;
class AActor;

USTRUCT(BlueprintType)
struct FKRHUDInitArgs // Args For HUD
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAbilitySystemComponent> ASC = nullptr;
};


UINTERFACE(BlueprintType)
class UKRHUDWidgetInterface : public UInterface { GENERATED_BODY() };


class KORAPROJECT_API IKRHUDWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void InitializeHUD(const FKRHUDInitArgs& Args);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
	void TearDownHUD();
};
