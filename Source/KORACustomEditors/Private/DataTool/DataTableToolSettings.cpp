#include "DataTool/DataTableToolSettings.h"
#include "Misc/Paths.h"

UDataTableToolSettings::UDataTableToolSettings()
{
	CSVFolderPath.Path = TEXT("/Game/Data/CSV");
	DataTableOutputFolderPath.Path = TEXT("/Game/Data/DataTables");
	StructNameSuffix = TEXT("Struct");
	StructNamePrefix = TEXT("");
}

void UDataTableToolSettings::PostInitProperties()
{
	Super::PostInitProperties();
}

#if WITH_EDITOR
void UDataTableToolSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif
