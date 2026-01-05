// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/KRCitizenStreamingSubsystem.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UKRCitizenStreamingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bPreloadCompleted = false;
	CachedRows.Reset();
	CachedRowNames.Reset();
	PreloadAssetList.Reset();

	/*if (!MHCitizenAppearanceTable && MHCitizenAppearanceTablePath.IsValid())
	{
		UObject* Loaded = MHCitizenAppearanceTablePath.TryLoad();
		MHCitizenAppearanceTable = Cast<UDataTable>(Loaded);

		if (!MHCitizenAppearanceTable)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("KRCitizenStreamingSubsystem: Failed to load DataTable from %s"),
				*MHCitizenAppearanceTablePath.ToString());
		}
	}*/
}

void UKRCitizenStreamingSubsystem::Deinitialize()
{
	CachedRows.Reset();
	CachedRowNames.Reset();
	PreloadAssetList.Reset();

	Super::Deinitialize();
}

void UKRCitizenStreamingSubsystem::SetAppearanceTable(UDataTable* InTable, bool bPreload)
{
	MHCitizenAppearanceTable = InTable;

	if (bPreload && MHCitizenAppearanceTable)
	{
		PreloadAppearances();
	}
}

const FCitizenDataStruct* UKRCitizenStreamingSubsystem::GetAppearanceRow(FName RowName) const
{
	if (const FCitizenDataStruct* const* Found = CachedRows.Find(RowName))
	{
		return *Found;
	}

	return MHCitizenAppearanceTable ? MHCitizenAppearanceTable->FindRow<FCitizenDataStruct>(RowName, TEXT("GetAppearanceRow")) : nullptr;
}

FName UKRCitizenStreamingSubsystem::GetRandomRowName() const
{
	if (CachedRowNames.Num() == 0)
	{
		return NAME_None;
	}

	const int32 Index = FMath::RandRange(0, CachedRowNames.Num() - 1);
	return CachedRowNames[Index];
}

void UKRCitizenStreamingSubsystem::PreloadAppearances()
{
	bPreloadCompleted = false;
	CachedRows.Reset();
	CachedRowNames.Reset();
	PreloadAssetList.Reset();

	if (!MHCitizenAppearanceTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("KRCitizenStreamingSubsystem: MHCitizenAppearanceTable is null."));
		OnPreloadCompleted();
		return;
	}

	TArray<FName> RowNames;
	{
		const TMap<FName, uint8*>& RowMap = MHCitizenAppearanceTable->GetRowMap();
		RowMap.GetKeys(RowNames);
	}

	auto AddSoftPath = [this](const FSoftObjectPath& Path)
		{
			if (!Path.IsNull())
			{
				PreloadAssetList.AddUnique(Path);
			}
		};

	for (const FName& RowName : RowNames)
	{
		if (const FCitizenDataStruct* Row = MHCitizenAppearanceTable->FindRow<FCitizenDataStruct>(RowName, TEXT("PreloadAppearances")))
		{
			CachedRows.Add(RowName, Row);
			CachedRowNames.Add(RowName);

			AddSoftPath(Row->BodyMesh.ToSoftObjectPath());
			AddSoftPath(Row->ClothMesh.ToSoftObjectPath());
			AddSoftPath(Row->FaceMesh.ToSoftObjectPath());

			for (const FCitizenGroomSlotData& G : Row->GroomSlots)
			{
				AddSoftPath(G.GroomAsset.ToSoftObjectPath());
				AddSoftPath(G.BindingAsset.ToSoftObjectPath());
			}
		}
	}

	if (PreloadAssetList.Num() > 0)
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

		Streamable.RequestAsyncLoad(
			PreloadAssetList,
			FStreamableDelegate::CreateUObject(
				this,
				&UKRCitizenStreamingSubsystem::OnPreloadCompleted));

		UE_LOG(LogTemp, Log, TEXT("KRCitizenStreamingSubsystem: RequestAsyncLoad for %d assets"), PreloadAssetList.Num());
	}
	else
	{
		OnPreloadCompleted();
	}
}

void UKRCitizenStreamingSubsystem::OnPreloadCompleted()
{
	bPreloadCompleted = true;
	UE_LOG(LogTemp, Warning, TEXT("[CitizenStream] Preload COMPLETED! Cached Rows = %d"), CachedRows.Num());
}
