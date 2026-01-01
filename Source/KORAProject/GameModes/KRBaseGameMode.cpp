// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/KRBaseGameMode.h"

#include "KRBaseGameState.h"
#include "KRExperienceDefinition.h"
#include "KRExperienceManagerComponent.h"
#include "Characters/KRHeroCharacter.h"
#include "Data/DataAssets/KRPawnData.h"
#include "Components/KRPawnExtensionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/KRPlayerController.h"
#include "Player/KRPlayerState.h"
#include "SubSystem/KRDataAssetRegistry.h"
#include "KRWorldSettings.h"

AKRBaseGameMode::AKRBaseGameMode()
{
	GameStateClass = AKRBaseGameState::StaticClass();
	PlayerControllerClass = AKRPlayerController::StaticClass();
	PlayerStateClass = AKRPlayerState::StaticClass();
	DefaultPawnClass = AKRHeroCharacter::StaticClass();
}

void AKRBaseGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectionOne);
}

void AKRBaseGameMode::InitGameState()
{
	Super::InitGameState();

	UKRExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UKRExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnKRExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* AKRBaseGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UKRPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

const UKRPawnData* AKRBaseGameMode::GetPawnDataForController(const AController* Controller) const
{
	if (Controller)
	{
		if (const AKRPlayerState* KRPS = Controller->GetPlayerState<AKRPlayerState>())
		{
			if (const UKRPawnData* PawnData = KRPS->GetPawnData<UKRPawnData>())
			{
				return PawnData;
			}
		}
	}

	check(GameState);
	UKRExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UKRExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UKRExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	return nullptr;
}

void AKRBaseGameMode::RegisterDataAssets()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRBaseGameMode] Failed to get GameInstance for DataAsset registration"));
		return;
	}

	UKRDataAssetRegistry* Registry = GameInstance->GetSubsystem<UKRDataAssetRegistry>();
	if (!Registry)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRBaseGameMode] Failed to get KRDataAssetRegistry subsystem"));
		return;
	}
	
	FKRDataAssetRegistrationResult Result = Registry->RegisterAllFromDataTablesSubsystem();
}

void AKRBaseGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* AKRBaseGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UKRPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}
	
	return nullptr;
}

void AKRBaseGameMode::HandleMatchAssignmentIfNotExpectionOne()
{
	FPrimaryAssetId ExperienceId;
	UWorld* World = GetWorld();
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UKRExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}
	
	if (!ExperienceId.IsValid())
	{
		if (AKRWorldSettings* TypedWorldSettings = Cast<AKRWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
		}
	}

	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("KRExperienceDefinition"), FName("BP_KRDefaultExperienceDefinition"));
	}

	OnMatchAssignmentGiven(ExperienceId);
}

void AKRBaseGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	check(ExperienceId.IsValid());

	UKRExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UKRExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->SetCurrentExperience(ExperienceId);
}

bool AKRBaseGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UKRExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UKRExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AKRBaseGameMode::OnExperienceLoaded(const UKRExperienceDefinition* CurrentExperience)
{
	RegisterDataAssets();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}

		if (PC && PC->IsLocalController())
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
	}
}