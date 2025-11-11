// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"

#include "Engine/LocalPlayer.h"
#include "Engine/GameInstance.h"

#include "CommonActivatableWidget.h"
#include "GameFramework/PlayerController.h"
#include "Input/CommonUIInputTypes.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

FUIActionBindingHandle UKRUIInputSubsystem::BindRow(UCommonActivatableWidget* Widget, FName RowName, FSimpleDelegate Handler, bool bShowInBar, bool bConsume)
{
    if (!Widget || !InputActionDataTable) return {};
    EnsureLifecycleHook(Widget);

    FSimpleDelegate Wrapped = FSimpleDelegate::CreateLambda([W = TWeakObjectPtr<UCommonActivatableWidget>(Widget),
        RN = RowName,
        H = MoveTemp(Handler)]() mutable { if (H.IsBound()) H.Execute(); });

    FBindUIActionArgs Args(MakeRow(RowName), bShowInBar, MoveTemp(Wrapped));
    Args.bConsumeInput = bConsume;

    FUIActionBindingHandle Handle = Widget->RegisterUIActionBinding(Args);
    Handles.FindOrAdd(Widget).Add(Handle);
    return Handle;
}

FKRUIBindingHandle UKRUIInputSubsystem::BindRow_BP(UCommonActivatableWidget* Widget, FName RowName, FKRUISimpleDynDelegate Handler, bool bShowInBar, bool bConsume)
{
    FKRUIBindingHandle Out;
    if (!Widget) return Out;

    FSimpleDelegate Simple = MakeSimpleDelegateFromDynamic(Handler);
    Out.Native = BindRow(Widget, RowName, MoveTemp(Simple), bShowInBar, bConsume);
    return Out;
}

void UKRUIInputSubsystem::BindBackDefault(UCommonActivatableWidget* Widget, FName RouteName)
{
    if (!Widget) return;

    FSimpleDelegate Handler = FSimpleDelegate::CreateWeakLambda(Widget, [Widget, RouteName]()
        {
            if (!Widget) return;

            if (UGameInstance* GI = Widget->GetGameInstance())
                if (auto* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
                {
                    if (RouteName != NAME_None)
                    {
                        Router->CloseRoute(RouteName);
                    }
                    else
                    {
                        Widget->DeactivateWidget();
                    }
                    return;
                }
            Widget->DeactivateWidget();
        });

    BindRow(Widget, Rows.Back /* TEXT("Back") */, MoveTemp(Handler), true, true);
}

void UKRUIInputSubsystem::UnbindAll(UCommonActivatableWidget* Widget)
{
    if (!Widget) return;

    if (TArray<FUIActionBindingHandle>* Arr = Handles.Find(Widget))
    {
        for (FUIActionBindingHandle& H : *Arr)
        {
            if (H.IsValid()) H.Unregister();
        }
        Arr->Reset();
    }
}

void UKRUIInputSubsystem::EnterUIMode(EMouseCaptureMode Capture, bool bShowCursor)
{
    ++UIModeRefCount;
    LastCapture = Capture;
    bDesiredShowCursor = bShowCursor;
    UpdateUIMode();
}

void UKRUIInputSubsystem::ReleaseUIMode()
{
    UIModeRefCount = FMath::Max(0, UIModeRefCount - 1);
    UpdateUIMode();
}

void UKRUIInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (!InputActionDataTable.IsNull() && !InputActionDataTable.IsValid())
    {
        InputActionDataTable.LoadSynchronous();
    }

    if (!InputActionDataTable.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("InputActionDataTable is NOT set. Please set in DefaultGame.ini [/Script/KORAProject.KRUIInputSubsystem]"));
    }

    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UGameInstance* GI = LP->GetGameInstance())
        {
            if (auto* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
            {
                Router->OnRouteOpened.AddDynamic(this, &ThisClass::OnRouterRouteOpened);
                Router->OnRouteClosed.AddDynamic(this, &ThisClass::OnRouterRouteClosed);
            }
        }
    }
}

void UKRUIInputSubsystem::Deinitialize()
{
    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UGameInstance* GI = LP->GetGameInstance())
        {
            if (auto* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
            {
                Router->OnRouteOpened.RemoveAll(this);
                Router->OnRouteClosed.RemoveAll(this);
            }
        }
    }
    Super::Deinitialize();
}

FDataTableRowHandle UKRUIInputSubsystem::MakeRow(FName RowName) const
{
    FDataTableRowHandle H;
    H.DataTable = InputActionDataTable.Get();
    H.RowName = RowName;
    return H;
}

void UKRUIInputSubsystem::EnsureLifecycleHook(UCommonActivatableWidget* Widget)
{
    if (!Handles.Contains(Widget))
    {
        Handles.Add(Widget, {});
        Widget->OnDeactivated().AddWeakLambda(this, [this, Widget]
            {
                UnbindAll(Widget);
                Handles.Remove(Widget);
            });
    }
}

void UKRUIInputSubsystem::ApplyEnable(EMouseCaptureMode Capture, bool bShowCursor)
{
    if (APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
    {
        PC->SetShowMouseCursor(bShowCursor);
        
        FInputModeGameAndUI Mode;
        Mode.SetHideCursorDuringCapture(false);
        //Mode.SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Mode);

        if (UITag.IsValid())
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn))
                {
                    ASC->AddLooseGameplayTag(UITag);
                }
            }
        }
    }
}

void UKRUIInputSubsystem::ApplyDisable()
{
    if (APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
    {
        PC->SetShowMouseCursor(false);

        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);

        if (UITag.IsValid())
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn))
                {
                    ASC->RemoveLooseGameplayTag(UITag);
                }
            }
        }
    }
}

bool UKRUIInputSubsystem::IsAnyNonGameLayerOpen() const
{
    const ULocalPlayer* LP = GetLocalPlayer();
    if (!LP) return false;

    const UGameInstance* GI = LP->GetGameInstance();
    if (!GI) return false;

    if (const auto* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
    {
        return  Router->GetActiveOnLayer(TEXT("GameMenu")) != nullptr
            || Router->GetActiveOnLayer(TEXT("Menu")) != nullptr
            || Router->GetActiveOnLayer(TEXT("Modal")) != nullptr;
    }
    return false;
}

void UKRUIInputSubsystem::UpdateUIMode()
{
    const bool bShouldBeActive = (UIModeRefCount > 0) || IsAnyNonGameLayerOpen();

    if (bShouldBeActive && !bUIModeApplied)
    {
        ApplyEnable(LastCapture, bDesiredShowCursor);
        bUIModeApplied = true;
    }
    else if (!bShouldBeActive && bUIModeApplied)
    {
        ApplyDisable();
        bUIModeApplied = false;
    }
}

FSimpleDelegate UKRUIInputSubsystem::MakeSimpleDelegateFromDynamic(const FKRUISimpleDynDelegate& Dyn)
{
    if (!Dyn.IsBound())
        return FSimpleDelegate();

    FKRUISimpleDynDelegate Local = Dyn;
    return FSimpleDelegate::CreateLambda([Local]() mutable
        {
            Local.ExecuteIfBound();
        });
}

void UKRUIInputSubsystem::OnRouterRouteOpened(ULocalPlayer* LP, FName Route, EKRUILayer Layer)
{
    if (LP != GetLocalPlayer()) return;
    UpdateUIMode();
}

void UKRUIInputSubsystem::OnRouterRouteClosed(ULocalPlayer* LP, FName Route, EKRUILayer Layer)
{
    if (LP != GetLocalPlayer()) return;
    UpdateUIMode();
}
