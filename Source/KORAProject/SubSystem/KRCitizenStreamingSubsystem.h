// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/KRCitizenAppearanceDataStruct.h"
#include "KRCitizenStreamingSubsystem.generated.h"


//UCLASS(Config = Game, DefaultConfig)
UCLASS()
class KORAPROJECT_API UKRCitizenStreamingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//UPROPERTY(EditDefaultsOnly, Config, Category = "Citizen|Appearance", meta = (AllowedClasses = "DataTable"))
	//FSoftObjectPath MHCitizenAppearanceTablePath; DT넣고 config로 설정해야함 

	UPROPERTY(Transient)
	UDataTable* MHCitizenAppearanceTable = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Citizen|Appearance")
	void SetAppearanceTable(UDataTable* InTable, bool bPreload = true);

	const FKRCitizenAppearanceDataStruct* GetAppearanceRow(FName RowName) const;

	UFUNCTION(BlueprintCallable, Category = "Citizen|Appearance")	FName GetRandomRowName() const;
	UFUNCTION(BlueprintPure, Category = "Citizen|Appearance")		bool IsPreloadCompleted() const { return bPreloadCompleted; }
	UFUNCTION(BlueprintCallable, Category = "Citizen|Appearance")	void PreloadAppearances();

private:
	void OnPreloadCompleted();

	bool bPreloadCompleted = false;

	TMap<FName, const FKRCitizenAppearanceDataStruct*> CachedRows;
	TArray<FName> CachedRowNames;
	TArray<FSoftObjectPath> PreloadAssetList;
};
