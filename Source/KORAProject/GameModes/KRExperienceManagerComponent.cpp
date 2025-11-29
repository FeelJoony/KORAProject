#include "GameModes/KRExperienceManagerComponent.h"
#include "GameFeaturesSubsystemSettings.h"
#include "System/KRAssetManager.h"
#include "GameModes/KRExperienceDefinition.h"

void UKRExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnKRExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(Delegate);
	}
}

void UKRExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UKRAssetManager& AssetManager = UKRAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);

	UObject* LoadedAsset = AssetPath.TryLoad();
	if (!LoadedAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load asset for ExperienceId: %s"), *ExperienceId.ToString());
		return;
	}

	const UKRExperienceDefinition* Experience = nullptr;
	
	if (UClass* LoadedClass = Cast<UClass>(LoadedAsset))
	{
		Experience = GetDefault<UKRExperienceDefinition>(LoadedClass);
	}
	else
	{
		Experience = Cast<UKRExperienceDefinition>(LoadedAsset);
	}
	
	if (!Experience)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to resolve Experience! LoadedAsset is not a valid Definition. Id: %s"), *ExperienceId.ToString());
		return;
	}

	check(CurrentExperience == nullptr);
	{
		CurrentExperience = Experience;
	}

	StartExperienceLoad();
}

void UKRExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == EKRExperienceLoadState::Unloaded);

	LoadState = EKRExperienceLoadState::Loading;

	UKRAssetManager& AssetManager = UKRAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	TArray<FName> BundleToLoad;
	{
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsServer || (OwnerNetMode != NM_Client);
		if (bLoadClient)
		{
			BundleToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}
		if (bLoadServer)
		{
			BundleToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);	
		}
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundleToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
		{
			OnAssetsLoadedDelegate.ExecuteIfBound();
		}));
	}
}

void UKRExperienceManagerComponent::OnExperienceLoadComplete()
{
	static int32 OnExperienceLoadComplete_FrameNumber = 0;

	OnExperienceFullLoadComplete();
}

void UKRExperienceManagerComponent::OnExperienceFullLoadComplete()
{
	check(LoadState != EKRExperienceLoadState::Loaded);

	LoadState = EKRExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const UKRExperienceDefinition* UKRExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EKRExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);

	return CurrentExperience;
}
