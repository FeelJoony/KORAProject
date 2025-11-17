// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PrimaryLayout/KRPrimaryGameLayout.h"
#include "UI/Data/KRUIRouteTable.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "Engine/GameInstance.h"

void UKRPrimaryGameLayout::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (UGameInstance* GI = GetGameInstance())
        if (auto* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
        {
            Router->RegisterLayer(TEXT("Game"), GameLayerStack);
            Router->RegisterLayer(TEXT("GameMenu"), GameMenuLayerStack);
            Router->RegisterLayer(TEXT("Menu"), MenuLayerStack);
            Router->RegisterLayer(TEXT("Modal"), ModalLayerStack);

            if (!RouteTable.IsNull())
            {
                if (UKRUIRouteTable* Table = RouteTable.LoadSynchronous())
                {
                    for (const FKRUIRouteRow& Row : Table->Routes)
                    {
                        if (Row.WidgetClass.IsNull())
                        {
                            UE_LOG(LogTemp, Warning, TEXT("[PrimaryLayout] Route '%s' has no WidgetClass skipped."), *Row.RouteName.ToString());
                            continue;
                        }

                        FKRRouteSpec Spec;
                        EKRUILayer ParsedLayer = EKRUILayer::GameMenu;

                        if (!Router->TryParseLayer(Row.Layer, ParsedLayer))
                        {
                            UE_LOG(LogTemp, Warning, TEXT("[PrimaryLayout] Unknown layer alias '%s' for route '%s' defaulting to GameMenu."), *Row.Layer.ToString(), *Row.RouteName.ToString());
                        }

                        Spec.Layer = ParsedLayer;
                        Spec.WidgetClass = Row.WidgetClass;
                        Spec.GameStopPolicy = Row.Policy;

                        Router->RegisterRoute(Row.RouteName, Spec);
                    }
                }
            }
        }
}