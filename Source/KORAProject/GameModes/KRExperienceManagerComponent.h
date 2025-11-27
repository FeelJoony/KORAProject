#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "KRExperienceManagerComponent.generated.h"

enum class EKRExperienceLoadState
{
	Unloaded,
	Loading,
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
	
	bool IsExperienceLoaded() { return (LoadState == EKRExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	void CallOrRegister_OnExperienceLoaded(FOnKRExperienceLoaded::FDelegate&& Delegate);

	void SetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnExperienceFullLoadComplete();
	const class UKRExperienceDefinition* GetCurrentExperienceChecked() const;
};
