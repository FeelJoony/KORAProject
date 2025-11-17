// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "KRUIRouteTable.generated.h"

USTRUCT(BlueprintType)
struct FKRUIRouteRow
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere) FName RouteName;
    UPROPERTY(EditAnywhere) FName Layer; // "Game" / "GameMenu" / "Menu" / "Modal"
    UPROPERTY(EditAnywhere) TSoftClassPtr<UCommonActivatableWidget> WidgetClass;
    UPROPERTY(EditAnywhere) EKRUIGameStopPolicy Policy = EKRUIGameStopPolicy::None;
};

UCLASS(BlueprintType)
class UKRUIRouteTable : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere) TArray<FKRUIRouteRow> Routes;
};