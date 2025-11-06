// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "KRUIRouterSubsystem.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;


UENUM(BlueprintType)
enum class EKRUIGameStopPolicy : uint8
{
	None,
	PauseWhileOpen,			// RefCount가 1일 때 게임 정지
	PauseWhileTop			// 위젯 활성화 (Top) 시 게임 정지
};

USTRUCT(BlueprintType)
struct FKRRouteSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName StackName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UCommonActivatableWidget> WidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EKRUIGameStopPolicy GameStopPolicy = EKRUIGameStopPolicy::None;
};

UCLASS()
class KORAPROJECT_API UKRUIRouterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UIRouter") void RegisterStack(FName StackName, UCommonActivatableWidgetStack* Stack);
	UFUNCTION(BlueprintCallable, Category = "UIRouter") void RegisterRoute(FName Route, const FKRRouteSpec& Spec);

	UFUNCTION(BlueprintCallable, Category = "UIRouter") UCommonActivatableWidget* ToggleRoute(FName Route);
	UFUNCTION(BlueprintCallable, Category = "UIRouter") UCommonActivatableWidget* OpenRoute(FName Route);
	UFUNCTION(BlueprintCallable, Category = "UIRouter") bool CloseRoute(FName Route);

	UFUNCTION(BlueprintCallable, Category = "UIRouter") UCommonActivatableWidget* GetActiveUIStack(FName StackName) const;
	UFUNCTION(BlueprintCallable, Category = "UIRouter") bool IsRouteOpen(FName Route) const;

private:
	TMap<FName, TWeakObjectPtr<UCommonActivatableWidgetStack>> UIStacks;
	TMap<FName, FKRRouteSpec> Routes;
	UPROPERTY() TMap<FName, int32> RouteRefCounts;
	UPROPERTY() TMap<FName, TWeakObjectPtr<UCommonActivatableWidget>> ActiveWidgets;

	int32 GlobalStopGameRefCount = 0;

	void UpdateGameStopState(UWorld* World);
	void ReleaseGameStopState(UWorld* World);
	void BindLifecycle(FName Route, UCommonActivatableWidget* W, const FKRRouteSpec& Spec);
};
