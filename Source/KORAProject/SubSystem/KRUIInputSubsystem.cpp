// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "CommonActivatableWidget.h"
#include "GameFramework/PlayerController.h"
#include "Input/CommonUIInputTypes.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

FUIActionBindingHandle UKRUIInputSubsystem::BindRow(
    UCommonActivatableWidget* Widget, FName RowName, FSimpleDelegate Handler, bool bShowInBar, bool bConsume)
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
    if (++UIModeRefCount == 1)
    {
        ApplyEnable(Capture, bShowCursor);
    }
}

void UKRUIInputSubsystem::ReleaseUIMode()
{
    if (UIModeRefCount > 0 && --UIModeRefCount == 0)
    {
        ApplyDisable();
    }
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
        UE_LOG(LogTemp, Error, TEXT("InputActionDataTable is NOT set. "
            "Please set in DefaultGame.ini [/Script/KORAProject.KRUIInputSubsystem]"));
    }
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
