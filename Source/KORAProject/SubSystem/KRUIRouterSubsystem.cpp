// Fill out your copyright notice in the Description page of Project Settings.


#include "KRUIRouterSubsystem.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Kismet/GameplayStatics.h"

//#include "SubSystem/KRUIInputSubsystem.h"
//DEFINE_LOG_CATEGORY(LogKRUIInput);

void UKRUIRouterSubsystem::RegisterStack(FName StackName, UCommonActivatableWidgetStack* Stack)
{
	if (IsValid(Stack))
	{
		UIStacks.Add(StackName, Stack);
	}
}

void UKRUIRouterSubsystem::RegisterRoute(FName Route, const FKRRouteSpec& Spec)
{
	Routes.Add(Route, Spec);
	RouteRefCounts.FindOrAdd(Route) = 0; 
}

/*****		 Open / Toggle / Close		 *****/
UCommonActivatableWidget* UKRUIRouterSubsystem::OpenRoute(FName Route)
{
	const FKRRouteSpec* Spec = Routes.Find(Route);
	if (!Spec || !Spec->WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Router] OpenRoute(%s) FAILED: no spec or class"), *Route.ToString());
		return nullptr;
	}

	UCommonActivatableWidgetStack* Stack = nullptr;
	if (const TWeakObjectPtr<UCommonActivatableWidgetStack>* Found = UIStacks.Find(Spec->StackName))
	{
		Stack = Found->Get();
	}

	if (!Stack) return nullptr;

	int32& Ref = RouteRefCounts.FindOrAdd(Route);
	const bool bFirstOpen = (Ref == 0);

	UCommonActivatableWidget* W = nullptr;

	if (bFirstOpen)
	{
#if (ENGINE_MAJOR_VERSION >= 5)
		W = Stack->AddWidget(Spec->WidgetClass);
#else
		W = Stack->BP_AddWidget(Spec->WidgetClass);
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
		}
		BindLifecycle(Route, W, *Spec);
	}
	else
	{
		if (TWeakObjectPtr<UCommonActivatableWidget>* WPtr = ActiveWidgets.Find(Route))
		{
			W = WPtr->Get();
		}
	}

	++Ref;
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

	UCommonActivatableWidgetStack* Stack = nullptr;
	if (const TWeakObjectPtr<UCommonActivatableWidgetStack>* Found = UIStacks.Find(Spec->StackName))
	{
		Stack = Found->Get();
	}
	--(*RefPtr);

	if (*RefPtr == 0)
	{
		if (TWeakObjectPtr<UCommonActivatableWidget>* WPtr = ActiveWidgets.Find(Route))
		{
			if (UCommonActivatableWidget* W = WPtr->Get())
			{
				if (Spec->GameStopPolicy == EKRUIGameStopPolicy::PauseWhileOpen)
				{
					ReleaseGameStopState(W->GetWorld());
				}
				W->DeactivateWidget();
			}
		}
		ActiveWidgets.Remove(Route);
	}

	return true;
}

UCommonActivatableWidget* UKRUIRouterSubsystem::ToggleRoute(FName Route)
{
	return !IsRouteOpen(Route) ? OpenRoute(Route) : (CloseRoute(Route), nullptr);
}

/*****		 Widget Stack LookUp		 *****/
UCommonActivatableWidget* UKRUIRouterSubsystem::GetActiveUIStack(FName StackName) const
{
	if (const TWeakObjectPtr<UCommonActivatableWidgetStack>* Found = UIStacks.Find(StackName))
	{
		if (UCommonActivatableWidgetStack* Stack = Found->Get())
		{
			return Stack->GetActiveWidget();
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