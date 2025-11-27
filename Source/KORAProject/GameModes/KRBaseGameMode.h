// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KRBaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KORAPROJECT_API AKRBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKRBaseGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	void HandleMatchAssignmentIfNotExpectionOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const class UKRExperienceDefinition* CurrentExperience);
	const class UKRPawnData* GetPawnDataForController(const AController* Controller) const;
	
};
