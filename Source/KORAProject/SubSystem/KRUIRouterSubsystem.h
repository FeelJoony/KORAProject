// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "KRUIRouterSubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;

UENUM(BlueprintType)
enum class EKRUILayer : uint8 // Priority
{
	Game = 0, GameMenu = 1, Menu = 2, Modal = 3
};

UENUM(BlueprintType)
enum class EKRUIGameStopPolicy : uint8
{
	None, PauseWhileOpen, PauseWhileTop
};

// Some routes are intended for non-UI Mode access
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRouteOpened, ULocalPlayer*, LocalPlayer, FName, Route, EKRUILayer, Layer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRouteClosed, ULocalPlayer*, LocalPlayer, FName, Route, EKRUILayer, Layer);

USTRUCT(BlueprintType)
struct FKRRouteSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftClassPtr<UCommonActivatableWidget> WidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EKRUILayer Layer = EKRUILayer::GameMenu;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EKRUIGameStopPolicy GameStopPolicy = EKRUIGameStopPolicy::None;
};

UCLASS()
class KORAPROJECT_API UKRUIRouterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	/***** Register UI StackLayer/Route *****/
	UFUNCTION(BlueprintCallable, Category = "UIRouter") void RegisterLayer(FName LayerName, UCommonActivatableWidgetStack* Stack);
	UFUNCTION(BlueprintCallable, Category = "UIRouter") void RegisterRoute(FName Route, const FKRRouteSpec& Spec);

	/***** Open/Close/Toggle Routes*****/
	UFUNCTION(BlueprintCallable, Category = "UIRouter") UCommonActivatableWidget* OpenRoute(FName Route);
	UFUNCTION(BlueprintCallable, Category = "UIRouter") bool CloseRoute(FName Route);
	UFUNCTION(BlueprintCallable, Category = "UIRouter") UCommonActivatableWidget* ToggleRoute(FName Route);

	UFUNCTION(BlueprintCallable, Category = "UIRouter") UCommonActivatableWidget* GetActiveOnLayer(FName LayerName) const;
	UFUNCTION(BlueprintCallable, Category = "UIRouter") bool IsRouteOpen(FName Route) const;

	UPROPERTY(BlueprintAssignable, Category = "UIRouter|Events") FOnRouteOpened OnRouteOpened;
	UPROPERTY(BlueprintAssignable, Category = "UIRouter|Events") FOnRouteClosed OnRouteClosed;

private:
	TMap<EKRUILayer, TWeakObjectPtr<UCommonActivatableWidgetStack>> UILayerStacks;

	TMap<FName, FKRRouteSpec> Routes;
	UPROPERTY() TMap<FName, int32> RouteRefCounts;
	UPROPERTY() TMap<FName, TWeakObjectPtr<UCommonActivatableWidget>> ActiveWidgets;

	int32 GlobalStopGameRefCount = 0;

	void UpdateGameStopState(UWorld* World);
	void ReleaseGameStopState(UWorld* World);
	void BindLifecycle(FName Route, UCommonActivatableWidget* W, const FKRRouteSpec& Spec);
};
