// Fill out your copyright notice in the Description page of Project Settings.


#include "KRUIRouterSubsystem.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Kismet/GameplayStatics.h"

bool UKRUIRouterSubsystem::TryParseLayer(FName Name, EKRUILayer& OutLayer) const
{
	if (Name == "Game" || Name == "Layer.Game" || Name == "HUD" || Name == "GameLayer") { OutLayer = EKRUILayer::Game;     return true; }
	if (Name == "GameMenu" || Name == "Layer.GameMenu" || Name == "Inventory" || Name == "Equipment" || Name == "Shop") { OutLayer = EKRUILayer::GameMenu; return true; }
	if (Name == "Menu" || Name == "Layer.Menu" || Name == "MainMenu") { OutLayer = EKRUILayer::Menu;     return true; }
	if (Name == "Modal" || Name == "Layer.Modal" || Name == "Popup" || Name == "Dialog") { OutLayer = EKRUILayer::Modal;    return true; }
	return false;
}

void UKRUIRouterSubsystem::RegisterLayer(FName LayerName, UCommonActivatableWidgetStack* Stack)
{
	if (!IsValid(Stack)) return;

	EKRUILayer Layer;
	if (!TryParseLayer(LayerName, Layer))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Router] RegisterStack(%s) failed: unknown layer alias"), *LayerName.ToString());
		return;
	}

	UILayerStacks.Add(Layer, Stack);
}

void UKRUIRouterSubsystem::RegisterRoute(FName Route, const FKRRouteSpec& Spec)
{
	if (Spec.WidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Router] RegisterRoute(%s) FAILED: WidgetClass is null (soft path missing)"), *Route.ToString());
		return;
	}

	Routes.Add(Route, Spec);
	RouteRefCounts.FindOrAdd(Route) = 0; 
}

/*****		 Open / Toggle / Close		 *****/
UCommonActivatableWidget* UKRUIRouterSubsystem::OpenRoute(FName Route)
{
	const FKRRouteSpec* Spec = Routes.Find(Route);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Router] OpenRoute(%s) FAILED: no spec"), *Route.ToString());
		return nullptr;
	}

	UCommonActivatableWidgetStack* Stack = nullptr;
	if (const TWeakObjectPtr<UCommonActivatableWidgetStack>* Found = UILayerStacks.Find(Spec->Layer))
	{
		Stack = Found->Get();
	}
	if (!Stack)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Router] OpenRoute(%s) failed: no stack for Layer %d"), *Route.ToString(), (int32)Spec->Layer);
		return nullptr;
	}

	int32& Ref = RouteRefCounts.FindOrAdd(Route);
	const bool bFirstOpen = (Ref == 0) || !ActiveWidgets.FindRef(Route).IsValid();

	UCommonActivatableWidget* W = nullptr;

	if (bFirstOpen)
	{
		UClass* Loaded = Spec->WidgetClass.LoadSynchronous();
		if (!Loaded)
		{
			UE_LOG(LogTemp, Error, TEXT("[Router] OpenRoute(%s) FAILED: LoadSynchronous() returned null"), *Route.ToString());
			return nullptr;
		}
		TSubclassOf<UCommonActivatableWidget> ClassToAdd = Loaded;

#if (ENGINE_MAJOR_VERSION >= 5)
		W = Stack->AddWidget(ClassToAdd);
#else
		W = Stack->BP_AddWidget(ClassToAdd);
#endif
		if (!W)
		{
			UE_LOG(LogTemp, Error, TEXT("[Router] OpenRoute(%s) AddWidget FAILED"), *Route.ToString());
			return nullptr;
		}

		ActiveWidgets.Add(Route, W);
		if (Spec->GameStopPolicy == EKRUIGameStopPolicy::PauseWhileOpen)
		{
			UpdateGameStopState(W->GetWorld());
		};
		BindLifecycle(Route, W, *Spec);

		if (Spec->GameStopPolicy == EKRUIGameStopPolicy::PauseWhileTop)
		{
			if (W->IsActivated() || (Stack->GetActiveWidget() == W))
			{
				UpdateGameStopState(W->GetWorld());
			}
		}
	}
	else
	{
		if (TWeakObjectPtr<UCommonActivatableWidget>* WPtr = ActiveWidgets.Find(Route))
		{
			W = WPtr->Get();
		}
	}

	++Ref;
	ULocalPlayer* LP = Stack->GetOwningLocalPlayer();
	OnRouteOpened.Broadcast(LP, Route, Spec->Layer);
	return W;
}


bool UKRUIRouterSubsystem::CloseRoute(FName Route)
{
	const FKRRouteSpec* Spec = Routes.Find(Route);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Router] CloseRoute(%s) FAILED: no spec"), *Route.ToString());
		return false;
	}

	int32* RefPtr = RouteRefCounts.Find(Route);
	if (!RefPtr || *RefPtr <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Router] CloseRoute(%s) IGNORED: ref missing or <= 0"), *Route.ToString());
		return false;
	}

	--(*RefPtr);

	if (*RefPtr == 0)
	{
		if (TWeakObjectPtr<UCommonActivatableWidget>* WPtr = ActiveWidgets.Find(Route))
		{
			if (UCommonActivatableWidget* W = WPtr->Get())
			{
				W->DeactivateWidget();

				if (Spec->GameStopPolicy == EKRUIGameStopPolicy::PauseWhileOpen)
				{
					ReleaseGameStopState(W->GetWorld());
				}
			}
		}
		ActiveWidgets.Remove(Route);
	}

	UCommonActivatableWidgetStack* Stack = nullptr;
	if (const auto* Found = UILayerStacks.Find(Spec->Layer))
	{
		Stack = Found->Get();
	}
	ULocalPlayer* LP = Stack ? Stack->GetOwningLocalPlayer() : nullptr;
	OnRouteClosed.Broadcast(LP, Route, Spec->Layer);

	return true;
}

UCommonActivatableWidget* UKRUIRouterSubsystem::ToggleRoute(FName Route)
{
	return !IsRouteOpen(Route) ? OpenRoute(Route) : (CloseRoute(Route), nullptr);
}

/*****		 Widget Stack LookUp		 *****/
UCommonActivatableWidget* UKRUIRouterSubsystem::GetActiveOnLayer(FName LayerName) const
{
	EKRUILayer Layer;
	if (!TryParseLayer(LayerName, Layer)) return nullptr;

	if (const TWeakObjectPtr<UCommonActivatableWidgetStack>* Found = UILayerStacks.Find(Layer))
	{
		if (UCommonActivatableWidgetStack* S = Found->Get())
		{
			return S->GetActiveWidget();
		}
	}
	return nullptr;
}

bool UKRUIRouterSubsystem::IsRouteOpen(FName Route) const
{
	const int32* Ref = RouteRefCounts.Find(Route);
	const TWeakObjectPtr<UCommonActivatableWidget>* WPtr = ActiveWidgets.Find(Route);
	return (Ref && *Ref > 0 && WPtr && WPtr->IsValid());
}

/*****		 Pause Game					 *****/
void UKRUIRouterSubsystem::UpdateGameStopState(UWorld* World)
{
	++GlobalStopGameRefCount;
	if (World)
	{
		UGameplayStatics::SetGamePaused(World, true);
	}
}

void UKRUIRouterSubsystem::ReleaseGameStopState(UWorld* World)
{
	GlobalStopGameRefCount = FMath::Max(0, GlobalStopGameRefCount - 1);
	if (World && GlobalStopGameRefCount == 0)
	{
		UGameplayStatics::SetGamePaused(World, false);
	}
}

/*****		 Control Widget LifeCycle		*****/
void UKRUIRouterSubsystem::BindLifecycle(FName Route, UCommonActivatableWidget* W, const FKRRouteSpec& Spec)
{
	if (!W) return;

	if (Spec.GameStopPolicy == EKRUIGameStopPolicy::PauseWhileTop)
	{
		W->OnActivated().AddWeakLambda(this, [this, W]()
			{
				UpdateGameStopState(W->GetWorld());
			});
		W->OnDeactivated().AddWeakLambda(this, [this, W]()
			{
				ReleaseGameStopState(W->GetWorld());
			});
	}
}