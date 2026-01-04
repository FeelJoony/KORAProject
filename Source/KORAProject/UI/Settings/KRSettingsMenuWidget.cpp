#include "UI/Settings/KRSettingsMenuWidget.h"
#include "UI/Settings/KRSettingsTabBase.h"
#include "UI/Settings/KRSettingsTabButton.h"
#include "Settings/KRSettingsSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "CommonButtonBase.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"

void UKRSettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		SettingsSubsystem = GI->GetSubsystem<UKRSettingsSubsystem>();
		RouterSubsystem = GI->GetSubsystem<UKRUIRouterSubsystem>();
	}

	if (ULocalPlayer* LP = GetOwningLocalPlayer())
	{
		UIInputSubsystem = LP->GetSubsystem<UKRUIInputSubsystem>();
	}
	
	if (SettingsSubsystem)
	{
		SettingsSubsystem->OnLanguageChanged.AddDynamic(this, &UKRSettingsMenuWidget::OnLanguageChanged);
	}

	SetupTabButtons();
	BindActionButtons();
	BindInputActions();
}

void UKRSettingsMenuWidget::NativeDestruct()
{
	if (SettingsSubsystem)
	{
		SettingsSubsystem->OnLanguageChanged.RemoveDynamic(this, &UKRSettingsMenuWidget::OnLanguageChanged);
	}
	
	for (auto& Pair : TabWidgets)
	{
		if (Pair.Value)
		{
			Pair.Value->RemoveFromParent();
		}
	}
	TabWidgets.Empty();
	CurrentTabWidget = nullptr;
	for (auto& Button : TabButtons)
	{
		if (Button)
		{
			Button->RemoveFromParent();
		}
	}
	TabButtons.Empty();

	Super::NativeDestruct();
}

void UKRSettingsMenuWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	CurrentFocusRow = EKRSettingsFocusRow::TabButtons;
	CurrentFocusIndex = 0;

	SwitchToTab(EKRSettingsMenuTab::Video);
	UpdateFocusVisuals();
}

void UKRSettingsMenuWidget::NativeOnDeactivated()
{
	if (SettingsSubsystem && SettingsSubsystem->HasPendingChanges())
	{
		SettingsSubsystem->RevertPendingSettings();
	}
	Super::NativeOnDeactivated();
}

void UKRSettingsMenuWidget::SetupTabButtons()
{
	TabButtons.Empty();

	if (!TabButtonClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("KRSettingsMenuWidget: TabButtonClass is not set! Please set it in Blueprint."));
		return;
	}

	if (!TabButtonContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("KRSettingsMenuWidget: TabButtonContainer is not bound! Make sure you have a HorizontalBox named 'TabButtonContainer' in Blueprint."));
		return;
	}

	TArray<FText> Names = TabNames;
	if (Names.Num() == 0)
	{
		Names = {
			NSLOCTEXT("Settings", "VideoTab", "Video"),
			NSLOCTEXT("Settings", "GraphicsTab", "Graphics"),
			NSLOCTEXT("Settings", "ControlTab", "Control"),
			NSLOCTEXT("Settings", "AudioTab", "Audio"),
			NSLOCTEXT("Settings", "GameplayTab", "Gameplay")
		};
	}

	for (int32 i = 0; i < Names.Num() && i <= static_cast<int32>(EKRSettingsMenuTab::Gameplay); ++i)
	{
		UKRSettingsTabButton* NewButton = CreateWidget<UKRSettingsTabButton>(this, TabButtonClass);
		if (NewButton)
		{
			NewButton->SetTabText(Names[i]);

			UHorizontalBoxSlot* HBoxSlot = TabButtonContainer->AddChildToHorizontalBox(NewButton);
			if (HBoxSlot)
			{
				HBoxSlot->SetHorizontalAlignment(HAlign_Fill);
				HBoxSlot->SetVerticalAlignment(VAlign_Fill);
			}

			EKRSettingsMenuTab Tab = static_cast<EKRSettingsMenuTab>(i);
			NewButton->OnClicked().AddWeakLambda(this, [this, Tab]() { SwitchToTab(Tab); });

			TabButtons.Add(NewButton);
		}
	}
}

void UKRSettingsMenuWidget::BindActionButtons()
{
	ActionButtons.Empty();

	if (ApplyButton)
	{
		ApplyButton->OnClicked().AddWeakLambda(this, [this]() { ApplySettings(); });
		ActionButtons.Add(ApplyButton);
	}
	if (ResetButton)
	{
		ResetButton->OnClicked().AddWeakLambda(this, [this]() { ResetToDefaults(); });
		ActionButtons.Add(ResetButton);
	}
	if (BackButton)
	{
		BackButton->OnClicked().AddWeakLambda(this, [this]() { CloseMenu(); });
		ActionButtons.Add(BackButton);
	}
}

void UKRSettingsMenuWidget::BindInputActions()
{
	if (!UIInputSubsystem) return;
	
	UIInputSubsystem->BindRow(this, UIInputSubsystem->Rows.Prev,
		FSimpleDelegate::CreateUObject(this, &UKRSettingsMenuWidget::HandleNavigatePrev), true, true);
	UIInputSubsystem->BindRow(this, UIInputSubsystem->Rows.Next,
		FSimpleDelegate::CreateUObject(this, &UKRSettingsMenuWidget::HandleNavigateNext), true, true);
	UIInputSubsystem->BindRow(this, UIInputSubsystem->Rows.Decrease,
		FSimpleDelegate::CreateUObject(this, &UKRSettingsMenuWidget::HandleNavigateUp), true, true);
	UIInputSubsystem->BindRow(this, UIInputSubsystem->Rows.Increase,
		FSimpleDelegate::CreateUObject(this, &UKRSettingsMenuWidget::HandleNavigateDown), true, true);
	UIInputSubsystem->BindRow(this, UIInputSubsystem->Rows.Select,
		FSimpleDelegate::CreateUObject(this, &UKRSettingsMenuWidget::HandleSelect), true, true);
}

UKRSettingsTabBase* UKRSettingsMenuWidget::CreateTabWidget(EKRSettingsMenuTab Tab)
{
	TSubclassOf<UKRSettingsTabBase> TabClass = nullptr;

	switch (Tab)
	{
	case EKRSettingsMenuTab::Video:
		TabClass = VideoTabClass;
		break;
	case EKRSettingsMenuTab::Graphics:
		TabClass = GraphicsTabClass;
		break;
	case EKRSettingsMenuTab::Control:
		TabClass = ControlTabClass;
		break;
	case EKRSettingsMenuTab::Audio:
		TabClass = AudioTabClass;
		break;
	case EKRSettingsMenuTab::Gameplay:
		TabClass = GameplayTabClass;
		break;
	}

	if (!TabClass)
	{
		return nullptr;
	}

	UKRSettingsTabBase* NewWidget = CreateWidget<UKRSettingsTabBase>(this, TabClass);
	if (NewWidget)
	{
		NewWidget->OnSettingChanged().AddDynamic(this, &UKRSettingsMenuWidget::OnTabSettingChanged);

		if (TabContentContainer)
		{
			UOverlaySlot* OverlaySlot = TabContentContainer->AddChildToOverlay(NewWidget);
			if (OverlaySlot)
			{
				OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
				OverlaySlot->SetVerticalAlignment(VAlign_Fill);
			}
		}

		NewWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	return NewWidget;
}

void UKRSettingsMenuWidget::ShowTabWidget(EKRSettingsMenuTab Tab)
{
	if (CurrentTabWidget)
	{
		CurrentTabWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	TObjectPtr<UKRSettingsTabBase>* FoundWidget = TabWidgets.Find(Tab);
	if (!FoundWidget || !*FoundWidget)
	{
		UKRSettingsTabBase* NewWidget = CreateTabWidget(Tab);
		if (NewWidget)
		{
			TabWidgets.Add(Tab, NewWidget);
			FoundWidget = TabWidgets.Find(Tab);
		}
	}

	if (FoundWidget && *FoundWidget)
	{
		CurrentTabWidget = *FoundWidget;
		CurrentTabWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (SettingsSubsystem)
		{
			CurrentTabWidget->RefreshFromSettings(SettingsSubsystem->GetPendingSettings());
		}
	}
}

void UKRSettingsMenuWidget::SwitchToTab(EKRSettingsMenuTab Tab)
{
	CurrentTab = Tab;
	ShowTabWidget(Tab);

	int32 TabIndex = static_cast<int32>(Tab);
	for (int32 i = 0; i < TabButtons.Num(); ++i)
	{
		if (TabButtons[i])
		{
			TabButtons[i]->SetIsSelected(i == TabIndex);
		}
	}
}

void UKRSettingsMenuWidget::HandleNavigatePrev()
{
	switch (CurrentFocusRow)
	{
	case EKRSettingsFocusRow::TabButtons:
		if (TabButtons.Num() > 0)
		{
			CurrentFocusIndex = (CurrentFocusIndex - 1 + TabButtons.Num()) % TabButtons.Num();
			SwitchToTab(static_cast<EKRSettingsMenuTab>(CurrentFocusIndex));
		}
		break;

	case EKRSettingsFocusRow::ActionButtons:
		if (ActionButtons.Num() > 0)
		{
			CurrentFocusIndex = (CurrentFocusIndex - 1 + ActionButtons.Num()) % ActionButtons.Num();
			UpdateFocusVisuals();
		}
		break;

	case EKRSettingsFocusRow::TabContent:
		if (CurrentTabWidget)
		{
			CurrentTabWidget->HandleNavigatePrev();
		}
		break;
	}
}

void UKRSettingsMenuWidget::HandleNavigateNext()
{
	switch (CurrentFocusRow)
	{
	case EKRSettingsFocusRow::TabButtons:
		if (TabButtons.Num() > 0)
		{
			CurrentFocusIndex = (CurrentFocusIndex + 1) % TabButtons.Num();
			SwitchToTab(static_cast<EKRSettingsMenuTab>(CurrentFocusIndex));
		}
		break;

	case EKRSettingsFocusRow::ActionButtons:
		if (ActionButtons.Num() > 0)
		{
			CurrentFocusIndex = (CurrentFocusIndex + 1) % ActionButtons.Num();
			UpdateFocusVisuals();
		}
		break;

	case EKRSettingsFocusRow::TabContent:
		if (CurrentTabWidget)
		{
			CurrentTabWidget->HandleNavigateNext();
		}
		break;
	}
}

void UKRSettingsMenuWidget::HandleNavigateUp()
{
	switch (CurrentFocusRow)
	{
	case EKRSettingsFocusRow::TabButtons:
		break;

	case EKRSettingsFocusRow::TabContent:
		if (CurrentTabWidget && CurrentTabWidget->GetFocusedWidgetIndex() == 0)
		{
			SetFocusRow(EKRSettingsFocusRow::TabButtons);
			CurrentFocusIndex = static_cast<int32>(CurrentTab);
		}
		else if (CurrentTabWidget)
		{
			CurrentTabWidget->HandleNavigateUp();
		}
		break;

	case EKRSettingsFocusRow::ActionButtons:
		SetFocusRow(EKRSettingsFocusRow::TabContent);
		if (CurrentTabWidget)
		{
			int32 LastIndex = CurrentTabWidget->GetFocusableWidgetCount() - 1;
			if (LastIndex >= 0)
			{
				for (int32 i = 0; i < LastIndex; ++i)
				{
					CurrentTabWidget->HandleNavigateDown();
				}
			}
		}
		CurrentFocusIndex = 0;
		break;
	}
	UpdateFocusVisuals();
}

void UKRSettingsMenuWidget::HandleNavigateDown()
{
	switch (CurrentFocusRow)
	{
	case EKRSettingsFocusRow::TabButtons:
		SetFocusRow(EKRSettingsFocusRow::TabContent);
		CurrentFocusIndex = 0;
		break;

	case EKRSettingsFocusRow::TabContent:
		if (CurrentTabWidget)
		{
			int32 LastIndex = CurrentTabWidget->GetFocusableWidgetCount() - 1;
			if (CurrentTabWidget->GetFocusedWidgetIndex() >= LastIndex)
			{
				SetFocusRow(EKRSettingsFocusRow::ActionButtons);
				CurrentFocusIndex = 0;
			}
			else
			{
				CurrentTabWidget->HandleNavigateDown();
			}
		}
		else
		{
			SetFocusRow(EKRSettingsFocusRow::ActionButtons);
			CurrentFocusIndex = 0;
		}
		break;

	case EKRSettingsFocusRow::ActionButtons:
		break;
	}
	UpdateFocusVisuals();
}

void UKRSettingsMenuWidget::SetFocusRow(EKRSettingsFocusRow NewRow)
{
	CurrentFocusRow = NewRow;
	if (CurrentTabWidget)
	{
		CurrentTabWidget->SetFocused(NewRow == EKRSettingsFocusRow::TabContent);
	}
}

void UKRSettingsMenuWidget::HandleSelect()
{
	switch (CurrentFocusRow)
	{
	case EKRSettingsFocusRow::TabButtons:
		break;

	case EKRSettingsFocusRow::TabContent:
		if (CurrentTabWidget)
		{
			CurrentTabWidget->HandleSelect();
		}
		break;

	case EKRSettingsFocusRow::ActionButtons:
		if (ActionButtons.IsValidIndex(CurrentFocusIndex) && ActionButtons[CurrentFocusIndex])
		{
			if (CurrentFocusIndex == 0 && ApplyButton)
			{
				ApplySettings();
			}
			else if (CurrentFocusIndex == 1 && ResetButton)
			{
				ResetToDefaults();
			}
			else if (CurrentFocusIndex == 2 && BackButton)
			{
				CloseMenu();
			}
		}
		break;
	}
}

void UKRSettingsMenuWidget::UpdateFocusVisuals()
{
	for (int32 i = 0; i < TabButtons.Num(); ++i)
	{
		if (TabButtons[i])
		{
			bool bSelected = (CurrentFocusRow == EKRSettingsFocusRow::TabButtons && i == CurrentFocusIndex);
			TabButtons[i]->SetIsSelected(bSelected || i == static_cast<int32>(CurrentTab));
		}
	}

	for (int32 i = 0; i < ActionButtons.Num(); ++i)
	{
		if (ActionButtons[i])
		{
			bool bSelected = (CurrentFocusRow == EKRSettingsFocusRow::ActionButtons && i == CurrentFocusIndex);
			ActionButtons[i]->SetIsSelected(bSelected);
		}
	}
}

void UKRSettingsMenuWidget::OnTabSettingChanged()
{
	UpdateApplyButtonState();
}

void UKRSettingsMenuWidget::UpdateApplyButtonState()
{
	if (ApplyButton && SettingsSubsystem)
	{
		ApplyButton->SetIsInteractionEnabled(SettingsSubsystem->HasPendingChanges());
	}
}

void UKRSettingsMenuWidget::ApplySettings()
{
	if (SettingsSubsystem)
	{
		SettingsSubsystem->ApplyPendingSettings(GetOwningPlayer());
		UpdateApplyButtonState();
	}
}

void UKRSettingsMenuWidget::RevertSettings()
{
	if (SettingsSubsystem)
	{
		SettingsSubsystem->RevertPendingSettings();

		// Refresh current tab
		if (CurrentTabWidget)
		{
			CurrentTabWidget->RefreshFromSettings(SettingsSubsystem->GetPendingSettings());
		}

		UpdateApplyButtonState();
	}
}

void UKRSettingsMenuWidget::ResetToDefaults()
{
	if (SettingsSubsystem)
	{
		SettingsSubsystem->ResetToDefaults();
		if (CurrentTabWidget)
		{
			CurrentTabWidget->RefreshFromSettings(SettingsSubsystem->GetPendingSettings());
		}

		UpdateApplyButtonState();
	}
}

void UKRSettingsMenuWidget::CloseMenu()
{
	if (SettingsSubsystem)
	{
		SettingsSubsystem->RevertPendingSettings();
	}

	if (RouterSubsystem)
	{
		RouterSubsystem->CloseRoute(TEXT("Settings"));
	}
}

bool UKRSettingsMenuWidget::HasPendingChanges() const
{
	return SettingsSubsystem ? SettingsSubsystem->HasPendingChanges() : false;
}

void UKRSettingsMenuWidget::OnLanguageChanged(EKRLanguage NewLanguage)
{
	TWeakObjectPtr<UKRUIRouterSubsystem> WeakRouter = RouterSubsystem;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick([WeakRouter]()
		{
			if (UKRUIRouterSubsystem* Router = WeakRouter.Get())
			{
				Router->CloseRoute(TEXT("Settings"));
				Router->OpenRoute(TEXT("Settings"));
			}
		});
	}
}
