#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "KRExperienceManagerComponent.generated.h"

enum class EKRExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnKRExperienceLoaded, const class UKRExperienceDefinition*);

UCLASS()
class KORAPROJECT_API UKRExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<const class UKRExperienceDefinition> CurrentExperience;

	EKRExperienceLoadState LoadState = EKRExperienceLoadState::Unloaded;
	FOnKRExperienceLoaded OnExperienceLoaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;
	
	bool IsExperienceLoaded() { return (LoadState == EKRExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	void CallOrRegister_OnExperienceLoaded(FOnKRExperienceLoaded::FDelegate&& Delegate);

	void SetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadComplete();
	const class UKRExperienceDefinition* GetCurrentExperienceChecked() const;
};
