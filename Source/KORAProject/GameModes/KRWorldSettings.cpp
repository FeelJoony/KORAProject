// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/KRWorldSettings.h"
#include "Engine/AssetManager.h"
#include "Logging/MessageLog.h"

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRWorldSettings)

AKRWorldSettings::AKRWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FPrimaryAssetId AKRWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull())
	{
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());

		if (!Result.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings)"),
				*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
		}
	}
	return Result;
}

#if WITH_EDITOR
void AKRWorldSettings::CheckForErrors()
{
	Super::CheckForErrors();

	FMessageLog MapCheck("MapCheck");

	// You can add custom map validation here
	// For example, checking for specific actor types or configuration issues

	//@TODO: Validate that DefaultGameplayExperience points to a valid asset
	if (!DefaultGameplayExperience.IsNull())
	{
		FPrimaryAssetId ExperienceId = GetDefaultGameplayExperience();
		if (!ExperienceId.IsValid())
		{
			MapCheck.Error()
				->AddToken(FTextToken::Create(FText::FromString(FString::Printf(
					TEXT("World Settings has an invalid DefaultGameplayExperience: %s"),
					*DefaultGameplayExperience.ToString()))));
		}
	}
}
#endif
