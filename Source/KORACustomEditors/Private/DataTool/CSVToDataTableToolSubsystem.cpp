#include "DataTool/CSVToDataTableToolSubsystem.h"
#include "DataTool/DataTableToolSettings.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Engine/DataTable.h"
#include "IAssetTools.h"
#include "EditorAssetLibrary.h"
#include "Factories/DataTableFactory.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "DataTool/TableRowConvertFunctionContainer.h"
#include <fstream>
#include <string>

#define LOCTEXT_NAMESPACE "CSVToDataTableTool"

DEFINE_LOG_CATEGORY(LogCSVToDataTableTool);

void UCSVToDataTableToolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogCSVToDataTableTool, Log, TEXT("CSVToDataTableToolSubsystem Initialized."));
}

void UCSVToDataTableToolSubsystem::Deinitialize()
{
    UE_LOG(LogCSVToDataTableTool, Log, TEXT("CSVToDataTableToolSubsystem Deinitialized."));
    Super::Deinitialize();
}

void UCSVToDataTableToolSubsystem::ConvertAllCSVsToDataTables()
{
    UE_LOG(LogCSVToDataTableTool, Log, TEXT("Starting CSV to DataTable Conversion..."));

    const UDataTableToolSettings* ToolSettings = GetDefault<UDataTableToolSettings>();
    if (!ToolSettings)
    {
        UE_LOG(LogCSVToDataTableTool, Error, TEXT("Failed to load DataTableToolSettings. Please check Project Settings."));
        FNotificationInfo Info(LOCTEXT("FailedLoadSettings", "CSV to DataTable Tool: Failed to load settings!"));
        Info.ExpireDuration = 15.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
        return;
    }

    FString ProjectContentDir = FPaths::ProjectContentDir();
    FString FullCSVFolderPath = FPaths::Combine(ProjectContentDir, ToolSettings->CSVFolderPath.Path.Replace(TEXT("/Game/"), TEXT("")));
    FString DataTablePackagePath = ToolSettings->DataTableOutputFolderPath.Path;

    if (!FPaths::DirectoryExists(FullCSVFolderPath))
    {
        UE_LOG(LogCSVToDataTableTool, Error, TEXT("CSV folder does not exist: %s"), *FullCSVFolderPath);
        FNotificationInfo Info(FText::Format(LOCTEXT("CSVFolderNotFound", "CSV folder not found: {0}"), FText::FromString(FullCSVFolderPath)));
        Info.ExpireDuration = 15.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
        return;
    }

    if (!UEditorAssetLibrary::DoesDirectoryExist(DataTablePackagePath))
    {
        UE_LOG(LogCSVToDataTableTool, Warning, TEXT("Creating Data Table output directory: %s"), *DataTablePackagePath);
        if (!IFileManager::Get().MakeDirectory(*DataTablePackagePath, true))
        {
            UE_LOG(LogCSVToDataTableTool, Error, TEXT("Failed to create Data Table output directory: %s"), *DataTablePackagePath);
            FNotificationInfo Info(FText::Format(LOCTEXT("CreateFolderFailed", "Failed to create Data Table folder: {0}"), FText::FromString(DataTablePackagePath)));
            Info.ExpireDuration = 15.0f;
            FSlateNotificationManager::Get().AddNotification(Info);
            return;
        }
    }

    TArray<FString> CSVFileNames;
    IFileManager::Get().FindFiles(CSVFileNames, *FullCSVFolderPath, TEXT(".csv"));

    if (CSVFileNames.Num() == 0)
    {
        UE_LOG(LogCSVToDataTableTool, Warning, TEXT("No CSV files found in %s"), *FullCSVFolderPath);
        FNotificationInfo Info(FText::Format(LOCTEXT("NoCSVFound", "No CSV files found in: {0}"), FText::FromString(FullCSVFolderPath)));
        Info.ExpireDuration = 15.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
        return;
    }

    int32 ProcessedCount = 0;
    int32 FailedCount = 0;

    for (const FString& CSVFileName : CSVFileNames)
    {
        FString FullCSVFilePath = FPaths::Combine(FullCSVFolderPath, CSVFileName);
        FString DataTableName = FPaths::GetBaseFilename(CSVFileName);

        FString StructName = ToolSettings->StructNamePrefix + DataTableName + ToolSettings->StructNameSuffix;

        UE_LOG(LogCSVToDataTableTool, Log, TEXT("Processing CSV: %s, attempting to map to Struct: %s"), *FullCSVFilePath, *StructName);

        if (ProcessCSVFile(FullCSVFilePath, DataTableName, DataTablePackagePath, StructName))
        {
            ProcessedCount++;
        }
        else
        {
            FailedCount++;
        }
    }

    FNotificationInfo Info(FText::Format(LOCTEXT("ConversionSummary", "CSV to DataTable Conversion Finished. Processed: {0}, Failed: {1}"),
        FText::AsNumber(ProcessedCount), FText::AsNumber(FailedCount)));
    Info.ExpireDuration = 15.0f;
    Info.bUseSuccessFailIcons = true;
    Info.Image = (FailedCount == 0) ? FSlateIcon(FAppStyle::GetAppStyleSetName(), "Notification.Success").GetIcon() : FSlateIcon(FAppStyle::GetAppStyleSetName(), "Notification.Fail").GetIcon();
    FSlateNotificationManager::Get().AddNotification(Info);

    UE_LOG(LogCSVToDataTableTool, Log, TEXT("CSV to DataTable Conversion Finished. Processed: %d, Failed: %d."), ProcessedCount, FailedCount);
}

bool UCSVToDataTableToolSubsystem::ProcessCSVFile(const FString& InFullCSVFilePath, const FString& InDataTableName, const FString& InDataTablePackagePath, const FString& InStructName)
{
    FString FileContent;
    if (!ReadCSVFileWithSharedAccess(InFullCSVFilePath, FileContent))
    {
        UE_LOG(LogCSVToDataTableTool, Error, TEXT("Failed to load CSV file: %s"), *InFullCSVFilePath);
        return false;
    }

    UScriptStruct* RowStruct = FindStructByName(InStructName);
    if (!RowStruct)
    {
        UE_LOG(LogCSVToDataTableTool, Error, TEXT("Could not find struct '%s' for CSV file '%s'. Please ensure it exists and inherits from FTableRowBase."), *InStructName, *FPaths::GetBaseFilename(InFullCSVFilePath));
        FNotificationInfo Info(FText::Format(LOCTEXT("StructNotFound", "Struct '{0}' not found for '{1}'. Please create it!"), FText::FromString(InStructName), FText::FromString(FPaths::GetBaseFilename(InFullCSVFilePath))));
        Info.ExpireDuration = 15.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
        return false;
    }
    if (!RowStruct->IsChildOf(FTableRowBase::StaticStruct()))
    {
        UE_LOG(LogCSVToDataTableTool, Error, TEXT("Struct '%s' does not inherit from FTableRowBase. This is required for Data Tables. Skipping '%s'."), *InStructName, *FPaths::GetBaseFilename(InFullCSVFilePath));
        FNotificationInfo Info(FText::Format(LOCTEXT("NotFTableRowBase", "Struct '{0}' does not inherit from FTableRowBase for '{1}'. Required!"), FText::FromString(InStructName), FText::FromString(FPaths::GetBaseFilename(InFullCSVFilePath))));
        Info.ExpireDuration = 15.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
        return false;
    }

    FString DataTableAssetPath = FPaths::Combine(InDataTablePackagePath, InDataTableName);
    UDataTable* DataTableToUse = Cast<UDataTable>(UEditorAssetLibrary::LoadAsset(DataTableAssetPath));

    if (DataTableToUse)
    {
        /*TArray<FName> RowNames = DataTableToUse->GetRowNames();
        for (const auto& RowName : RowNames)
        {
            DataTableToUse->RemoveRow(RowName);
        }*/
        UE_LOG(LogCSVToDataTableTool, Log, TEXT("Found existing Data Table: %s. Updating..."), *DataTableAssetPath);
        if (DataTableToUse->RowStruct != RowStruct)
        {
            UE_LOG(LogCSVToDataTableTool, Warning, TEXT("Existing Data Table '%s' has a different RowStruct ('%s' vs '%s'). Attempting to set new RowStruct. Data might be corrupted!"), *DataTableAssetPath, *DataTableToUse->RowStruct->GetName(), *RowStruct->GetName());
            DataTableToUse->RowStruct = RowStruct;
        }
    }
    else
    {
        UE_LOG(LogCSVToDataTableTool, Log, TEXT("Creating new Data Table: %s"), *DataTableAssetPath);

        IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

        FString PackageName = DataTableAssetPath;
        UPackage* Package = CreatePackage(*PackageName);

        UDataTable* NewDataTable = NewObject<UDataTable>(Package, *InDataTableName, RF_Public | RF_Standalone);

        if (!NewDataTable)
        {
            UE_LOG(LogCSVToDataTableTool, Error, TEXT("Failed to create Data Table asset: %s"), *PackageName);
            FNotificationInfo Info(FText::Format(LOCTEXT("CreateDTFailed", "Failed to create Data Table: {0}"), FText::FromString(InDataTableName)));
            Info.ExpireDuration = 15.0f;
            Info.Image = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Notification.Fail").GetIcon();
            FSlateNotificationManager::Get().AddNotification(Info);
            return false;
        }

        NewDataTable->RowStruct = RowStruct;
        Package->MarkPackageDirty();
        DataTableToUse = NewDataTable;

        UE_LOG(LogCSVToDataTableTool, Log, TEXT("Data Table '%s' created with RowStruct '%s'."), *InDataTableName, *RowStruct->GetName());
    }

    FConverter_Params Params;
    Params.DataTable = DataTableToUse;
    Params.CSVString = FileContent;

    FString FunctionString = FString::Printf(TEXT("Create%s"), *InDataTableName);
    UFunction* ConvertFunction = UTableRowConvertFunctionContainer::StaticClass()->FindFunctionByName(*FunctionString);
    ProcessEvent(ConvertFunction, &Params);

    if (DataTableToUse->GetRowMap().Num() == 0)
    {
        UE_LOG(LogCSVToDataTableTool, Error, TEXT("Failed to import JSON data into Data Table '%s'. Please ensure the CSV headers match the '%s' struct members and types correctly."), *InDataTableName, *RowStruct->GetName());
        FNotificationInfo Info(FText::Format(LOCTEXT("ImportDTFailed", "Failed to import '{0}' data. Check CSV/Struct!"), FText::FromString(InDataTableName)));
        Info.ExpireDuration = 15.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
        return false;
    }

    if (!UEditorAssetLibrary::SaveAsset(DataTableAssetPath))
    {
        UE_LOG(LogCSVToDataTableTool, Error, TEXT("Failed to save Data Table asset: %s"), *DataTableAssetPath);
        FNotificationInfo Info(FText::Format(LOCTEXT("SaveDTFailed", "Failed to save Data Table: {0}"), FText::FromString(InDataTableName)));
        Info.ExpireDuration = 5.0f;
        FSlateNotificationManager::Get().AddNotification(Info);
        return false;
    }

    UE_LOG(LogCSVToDataTableTool, Log, TEXT("Successfully created/updated Data Table: %s"), *DataTableAssetPath);
    return true;
}


UScriptStruct* UCSVToDataTableToolSubsystem::FindStructByName(const FString& StructName)
{
    UScriptStruct* FoundStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *StructName);

    if (!FoundStruct)
    {
        FString ProjectName = FApp::GetProjectName();
        FString ScriptPath = FString::Printf(TEXT("/Script/%s.%s"), *ProjectName, *StructName);
        FoundStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *ScriptPath);
    }

    if (!FoundStruct)
    {
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        TArray<FAssetData> AssetData;
        AssetRegistryModule.Get().GetAssetsByClass(UScriptStruct::StaticClass()->GetClassPathName(), AssetData, true);

        for (const FAssetData& Data : AssetData)
        {
            if (Data.AssetName.ToString() == StructName || Data.AssetName.ToString() == StructName.Replace(TEXT("F"), TEXT(""), ESearchCase::CaseSensitive))
            {
                return Cast<UScriptStruct>(Data.GetAsset());
            }
        }
    }

    return FoundStruct;
}

bool UCSVToDataTableToolSubsystem::ReadCSVFileWithSharedAccess(const FString& FilePath, FString& OutString)
{
    std::ifstream Stream(TCHAR_TO_UTF8(*FilePath));
    if (!Stream.is_open())
    {
        UE_LOG(LogCSVToDataTableTool, Warning, TEXT("Failed to open file: %s"), *FilePath);
        return false;
    }

    std::string Content((std::istreambuf_iterator<char>(Stream)), std::istreambuf_iterator<char>());
    Stream.close();

    OutString = UTF8_TO_TCHAR(Content.c_str());
    return true;
}

#undef LOCTEXT_NAMESPACE
