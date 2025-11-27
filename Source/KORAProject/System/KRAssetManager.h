#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "KRAssetManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogKRAssetManager, Log, All);

UCLASS()
class KORAPROJECT_API UKRAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
// Constructor Section
public:
	UKRAssetManager();

	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	FCriticalSection SyncObject;

public:
	static UKRAssetManager& Get();

	virtual void StartInitialLoading() override;

	static bool ShouldLogAssetLoads();

	UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepsInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepsInMemory = true);

	void AddLoadedAsset(const UObject* Asset);

	
};

template <typename AssetType>
AssetType* UKRAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepsInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepsInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UKRAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepsInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepsInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}
