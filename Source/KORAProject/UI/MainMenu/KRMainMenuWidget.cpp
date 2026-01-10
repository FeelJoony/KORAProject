#include "UI/MainMenu/KRMainMenuWidget.h"
#include "UI/PauseMenu/KRMenuTabButton.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "Groups/CommonButtonGroupBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameModes/KRFrontendStateComponent.h"
#include "GameModes/KRBaseGameState.h"

void UKRMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	ButtonGroup->SetSelectionRequired(true);
	
	if (ContinueButton)
	{
		const bool bHasSave = false;

		ContinueButton->SetVisibility(bHasSave ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	
	InitializeMenuButtons();
	
	if (UWorld* World = GetWorld())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			FrontendStateComponent = GameState->FindComponentByClass<UKRFrontendStateComponent>();
		}
		
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UGameplayMessageSubsystem* MessageSubsystem = GI->GetSubsystem<UGameplayMessageSubsystem>())
			{
				ConfirmListenerHandle = MessageSubsystem->RegisterListener<FKRUIMessage_Confirm>(
					FKRUIMessageTags::Confirm(),
					this,
					&ThisClass::OnConfirmMessageReceived);
			}
		}
	}
	
	if (ContinueButton && ContinueButton->GetVisibility() == ESlateVisibility::Visible)
	{
		BindMenuButton(ContinueButton);
	}

	if (NewGameButton)
	{
		BindMenuButton(NewGameButton);
	}

	if (SettingButton)
	{
		BindMenuButton(SettingButton);
	}

	if (QuitGameButton)
	{
		BindMenuButton(QuitGameButton);
	}
}

void UKRMainMenuWidget::NativeDestruct()
{
	if (ConfirmListenerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (UGameplayMessageSubsystem* MessageSubsystem = GI->GetSubsystem<UGameplayMessageSubsystem>())
				{
					MessageSubsystem->UnregisterListener(ConfirmListenerHandle);
				}
			}
		}
	}

	Super::NativeDestruct();
}

void UKRMainMenuWidget::OnConfirmMessageReceived(FGameplayTag Channel, const FKRUIMessage_Confirm& Payload)
{
	if (Payload.Context == EConfirmContext::NewGame && Payload.Result == EConfirmResult::Yes)
	{
		if (FrontendStateComponent)
		{
			FrontendStateComponent->TravelToGameplay();
		}
	}
	else if (Payload.Context == EConfirmContext::QuitGame && Payload.Result == EConfirmResult::Yes)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
		}
	}
	else if (Payload.Context == EConfirmContext::Continue && Payload.Result == EConfirmResult::Yes)
	{
		if (FrontendStateComponent)
		{
			FrontendStateComponent->TravelToContinue();
		}
	}
	else if (Payload.Context == EConfirmContext::Generic && Payload.Result == EConfirmResult::Yes)
	{
		// Generic context from SettingButton - open Settings route
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				Router->OpenRoute(FName("Settings"));
			}
		}
	}
}

void UKRMainMenuWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDown));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
	}
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		UpdateMenuSelection(0);
	});
}

void UKRMainMenuWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	ULocalPlayer* LP = GetOwningLocalPlayer();
	if (!LP)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			LP = PC->GetLocalPlayer();
		}
	}
	
	if (!LP)
	{
		return;
	}
	
	if (UKRUIInputSubsystem* InputSubsys = LP->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->UnbindAll(this);
	}
}

void UKRMainMenuWidget::InitializeMenuButtons()
{
	MenuButtons.Empty();
	
	if (ContinueButton && ContinueButton->GetVisibility() == ESlateVisibility::Visible)
	{
		MenuButtons.Add(ContinueButton);
		ButtonGroup->AddWidget(ContinueButton);
	}

	if (NewGameButton)
	{
		MenuButtons.Add(NewGameButton);
		ButtonGroup->AddWidget(NewGameButton);
	}

	if (SettingButton)
	{
		MenuButtons.Add(SettingButton);
		ButtonGroup->AddWidget(SettingButton);
	}

	if (QuitGameButton)
	{
		MenuButtons.Add(QuitGameButton);
		ButtonGroup->AddWidget(QuitGameButton);
	}
}

void UKRMainMenuWidget::UpdateMenuSelection(int32 NewIndex)
{
	if (!MenuButtons.IsValidIndex(NewIndex))
	{
		return;
	}

	CurrentButtonIndex = NewIndex;
	
	if (ButtonGroup)
	{
		ButtonGroup->SelectButtonAtIndex(NewIndex);
	}
	
	if (UCommonButtonBase* SelectedButton = MenuButtons[NewIndex])
	{
		SelectedButton->SetFocus();
	}
}

void UKRMainMenuWidget::HandleMoveUp()
{
	int32 NewIndex = FMath::Max(0, CurrentButtonIndex - 1);
	if (NewIndex != CurrentButtonIndex)
	{
		UpdateMenuSelection(NewIndex);
	}
}

void UKRMainMenuWidget::HandleMoveDown()
{
	int32 NewIndex = FMath::Min(MenuButtons.Num() - 1, CurrentButtonIndex + 1);
	if (NewIndex != CurrentButtonIndex)
	{
		UpdateMenuSelection(NewIndex);
	}
}

void UKRMainMenuWidget::HandleSelect()
{
	if (!MenuButtons.IsValidIndex(CurrentButtonIndex))
	{
		return;
	}

	if (UCommonButtonBase* SelectedButton = MenuButtons[CurrentButtonIndex])
	{
		SelectedButton->OnClicked().Broadcast();
	}
}

void UKRMainMenuWidget::BindMenuButton(UCommonButtonBase* Button)
{
	if (!Button)
	{
		return;
	}

	// SettingButton 특별 처리 - 바로 Settings Route 열기 (PauseMenu 방식)
	if (Button == SettingButton)
	{
		Button->OnClicked().AddLambda([this]()
		{
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
				{
					Router->OpenRoute(FName("Settings"));
				}
			}
		});
	}
	else
	{
		Button->OnClicked().AddLambda([this, Button]()
		{
			EConfirmContext Context = GetContextForButton(Button);
			BP_OnMenuButtonInvoked(Context);
		});
	}

	// Bind hover to update selection (prevents dual selection visual)
	Button->OnHovered().AddLambda([this, Button]()
	{
		// Find the index of the hovered button and update selection
		int32 HoveredIndex = MenuButtons.IndexOfByKey(Button);
		if (HoveredIndex != INDEX_NONE)
		{
			UpdateMenuSelection(HoveredIndex);
		}
	});
}

EConfirmContext UKRMainMenuWidget::GetContextForButton(UCommonButtonBase* Button) const
{
	if (Button == NewGameButton)
	{
		return EConfirmContext::NewGame;
	}
	if (Button == ContinueButton)
	{
		return EConfirmContext::Continue;
	}
	if (Button == QuitGameButton)
	{
		return EConfirmContext::QuitGame;
	}
	if (Button == SettingButton)
	{
		return EConfirmContext::Generic;
	}

	return EConfirmContext::Generic;
}
