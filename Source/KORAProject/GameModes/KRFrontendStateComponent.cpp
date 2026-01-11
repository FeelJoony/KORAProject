#include "GameModes/KRFrontendStateComponent.h"
#include "GameModes/KRExperienceManagerComponent.h"
#include "GameModes/KRUserFacingExperience.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "SubSystem/KRMapTravelSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRFrontendStateComponent)

UKRFrontendStateComponent::UKRFrontendStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRFrontendStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);
	if (!MapName.Contains(TEXT("MainMenu")))
	{
		bHasShownMainMenu = true;
		OnFrontendReady.Broadcast();
		OnFrontendReadyEvent();
		return;
	}

	if (UGameInstance* GI = World->GetGameInstance())
	{
		if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			Router->OpenRoute(MainMenuRouteName);
			bHasShownMainMenu = true;
		}
	}
	
	OnFrontendReady.Broadcast();
	OnFrontendReadyEvent();
}

void UKRFrontendStateComponent::TravelToGameplay()
{
	if (UKRUserFacingExperience* Experience = DefaultGameplayExperience.LoadSynchronous())
	{
		if (UGameInstance* GI = GetWorld()->GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				Router->CloseRoute(MainMenuRouteName);
			}

			if (UKRMapTravelSubsystem* MapTravel =
				GI->GetSubsystem<UKRMapTravelSubsystem>())
			{
				MapTravel->TravelToExperience(
					Experience->GetPathName()
				);
				return;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("[FrontendState] MapTravelSubsystem missing"));
	}
}

void UKRFrontendStateComponent::TravelToContinue()
{
	if (HasSaveData())
	{
		TravelToGameplay();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[FrontendState] No save data found for continue"));
	}
}

bool UKRFrontendStateComponent::HasSaveData() const
{
	return UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0);
}

void UKRFrontendStateComponent::OnFrontendReadyEvent_Implementation()
{
	// Default implementation - can be overridden in Blueprint
}
