// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "KRHUDWidgetInterface.generated.h"

class UAbilitySystemComponent;
class AActor;

USTRUCT(BlueprintType)
struct FKRHUDInitArgs
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) TObjectPtr<UAbilitySystemComponent> ASC = nullptr;
	//UPROPERTY(BlueprintReadOnly) TObjectPtr<AActor> Avatar = nullptr;
};


UINTERFACE(BlueprintType)
class UKRHUDWidgetInterface : public UInterface { GENERATED_BODY() };


class KORAPROJECT_API IKRHUDWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void InitializeHUD(const FKRHUDInitArgs& Args) = 0;
	virtual void TearDownHUD() = 0;
};
