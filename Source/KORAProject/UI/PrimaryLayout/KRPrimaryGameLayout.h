// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "KRPrimaryGameLayout.generated.h"

class UCommonActivatableWidgetStack;
class UKRUIRouteTable;

UCLASS(BlueprintType, Blueprintable)
class KORAPROJECT_API UKRPrimaryGameLayout : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UCommonActivatableWidgetStack* GameLayerStack = nullptr;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UCommonActivatableWidgetStack* GameMenuLayerStack = nullptr;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UCommonActivatableWidgetStack* MenuLayerStack = nullptr;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UCommonActivatableWidgetStack* ModalLayerStack = nullptr;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UCommonActivatableWidgetStack* GamePopupLayerStack = nullptr;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UCommonActivatableWidgetStack* CinematicLayerStack = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Layout")
    UCommonActivatableWidgetStack* GetStack(EKRUILayer Layer) const
    {
        switch (Layer)
        {
        case EKRUILayer::Game:     return GameLayerStack;
        case EKRUILayer::GameMenu: return GameMenuLayerStack;
        case EKRUILayer::Menu:     return MenuLayerStack;
        case EKRUILayer::Modal:    return ModalLayerStack;
		case EKRUILayer::GamePopup:return GamePopupLayerStack;
        case EKRUILayer::Cinematic: return CinematicLayerStack;
        default: return nullptr;
        }
    }

protected:
    virtual void NativeOnInitialized() override;

    UPROPERTY(EditDefaultsOnly, Category = "Layout")
    TSoftObjectPtr<UKRUIRouteTable> RouteTable;
};