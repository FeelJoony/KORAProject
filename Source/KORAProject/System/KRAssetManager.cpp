#include "System/KRAssetManager.h"

DEFINE_LOG_CATEGORY(LogKRAssetManager);

UKRAssetManager::UKRAssetManager()
{
}

UKRAssetManager& UKRAssetManager::Get()
{
	check(GEngine);

	if (UKRAssetManager* Singleton = Cast<UKRAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogKRAssetManager, Fatal, TEXT("invalid AssetManagerClassname in DefaultEngine.ini(project settings); It must be KRAssetManager"));

	return *NewObject<UKRAssetManager>();
}

void UKRAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 엔진이 켜지기 맨 최초에 꼭 필요한 셋팅은 여기서 셋팅하는게 좋을 듯 하다.
}

bool UKRAssetManager::ShouldLogAssetLoads()
{
	const TCHAR* CommandLineContent = FCommandLine::Get();
	static bool bLogAssetLoads = FParse::Param(CommandLineContent, TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

UObject* UKRAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (::IsValid(GEngine->AssetManager))
		{
			return GetStreamableManager().LoadSynchronous(AssetPath);
		}

		return AssetPath.TryLoad();
	}

	return nullptr;
}

void UKRAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}
