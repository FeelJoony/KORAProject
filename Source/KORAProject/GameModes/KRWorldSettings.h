// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "KRWorldSettings.generated.h"

class UKRExperienceDefinition;

/**
 * The default world settings object for KORA project
 * Used primarily to set the default gameplay experience to use when playing on this map
 */
UCLASS()
class KORAPROJECT_API AKRWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	AKRWorldSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

public:
	// Returns the default experience to use when a server opens this map
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// The default experience to use when a server opens this map
	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	TSoftClassPtr<UKRExperienceDefinition> DefaultGameplayExperience;

public:
#if WITH_EDITORONLY_DATA
	// Is this level part of a front-end or other standalone experience?
	// When set, the net mode will be forced to Standalone when you hit Play in the editor
	UPROPERTY(EditDefaultsOnly, Category = PIE)
	bool ForceStandaloneNetMode = false;
#endif
};