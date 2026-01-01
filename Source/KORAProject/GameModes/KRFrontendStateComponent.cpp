#include "GameModes/KRFrontendStateComponent.h"
#include "GameModes/KRExperienceManagerComponent.h"
#include "GameModes/KRUserFacingExperience.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRFrontendStateComponent)

UKRFrontendStateComponent::UKRFrontendStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRFrontendStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Wait for experience to load
	if (AGameStateBase* GS = GetGameStateChecked<AGameStateBase>())
	{
		if (UKRExperienceManagerComponent* ExperienceManager = GS->FindComponentByClass<UKRExperienceManagerComponent>())
		{
			ExperienceManager->CallOrRegister_OnExperienceLoaded(
				FOnKRExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
		}
	}
}

void UKRFrontendStateComponent::OnExperienceLoaded(const UKRExperienceDefinition* Experience)
{
	bIsExperienceLoaded = true;
	TryShowMainMenu();
}

void UKRFrontendStateComponent::TryShowMainMenu()
{
	if (!bIsExperienceLoaded || bHasShownMainMenu)
	{
		return;
	}

	ShowMainMenu();
}

void UKRFrontendStateComponent::ShowMainMenu()
{
	if (bHasShownMainMenu)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Open main menu via UI Router
	if (UGameInstance* GI = World->GetGameInstance())
	{
		if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			Router->OpenRoute(MainMenuRouteName);
			bHasShownMainMenu = true;

			UE_LOG(LogTemp, Log, TEXT("[FrontendState] Main menu opened - input mode managed by UIRouter"));
		}
	}

	// Broadcast ready event
	OnFrontendReady.Broadcast();
	OnFrontendReadyEvent();
}

// SetUIOnlyInputMode removed - input mode is now managed by UIRouterSubsystem

void UKRFrontendStateComponent::TravelToGameplay()
{
	if (UKRUserFacingExperience* Experience = DefaultGameplayExperience.LoadSynchronous())
	{
		TravelToExperience(Experience);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[FrontendState] DefaultGameplayExperience is not set!"));
	}
}

void UKRFrontendStateComponent::TravelToContinue()
{
	// TODO: Load save data here before traveling
	// For now, just travel to gameplay
	if (HasSaveData())
	{
		// Load save game data
		// Then travel
		TravelToGameplay();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[FrontendState] No save data found for continue"));
	}
}

void UKRFrontendStateComponent::TravelToExperience(const UKRUserFacingExperience* Experience)
{
	if (!Experience)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FrontendState] TravelToExperience: Experience is null"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	APlayerController* HostingPlayer = World->GetFirstPlayerController();
	if (!HostingPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("[FrontendState] TravelToExperience: No valid PlayerController found!"));
		return;
	}

	if (UGameInstance* GI = World->GetGameInstance())
	{
		if (UCommonSessionSubsystem* SessionSubsystem = GI->GetSubsystem<UCommonSessionSubsystem>())
		{
			UCommonSession_HostSessionRequest* Request = Experience->CreateHostingRequest();
			if (Request)
			{
				UE_LOG(LogTemp, Log, TEXT("[FrontendState] Hosting session with MapID: %s"),
					*Experience->MapID.ToString());
				SessionSubsystem->HostSession(HostingPlayer, Request);
			}
		}
	}
}

bool UKRFrontendStateComponent::HasSaveData() const
{
	// TODO: Implement actual save data check
	// For now, check if a save game exists
	return UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0);
}

void UKRFrontendStateComponent::OnFrontendReadyEvent_Implementation()
{
	// Default implementation - can be overridden in Blueprint
}
