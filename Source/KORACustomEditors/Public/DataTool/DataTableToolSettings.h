#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DataTableToolSettings.generated.h"

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "CSV to DataTable Tool"))
class KORACUSTOMEDITORS_API UDataTableToolSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UDataTableToolSettings();

	UPROPERTY(EditAnywhere, Config, Category = "Paths", meta = (RelativeToGameContentDir, LongPackagePath))
	FDirectoryPath CSVFolderPath;

	UPROPERTY(EditAnywhere, Config, Category = "Paths", meta = (RelativeToGameContentDir, LongPackagePath))
	FDirectoryPath DataTableOutputFolderPath;

	UPROPERTY(EditAnywhere, Config, Category = "Naming Convention", meta = (ToolTip = "Suffix to append to CSV file name (e.g. 'MyGameData' + 'Struct' = 'FMyGameDataStruct'). Empty for no suffix."))
	FString StructNameSuffix;

	UPROPERTY(EditAnywhere, Config, Category = "Naming Convention", meta = (ToolTip = "Prefix for the UStruct name (e.g. 'F')."))
	FString StructNamePrefix;

	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
