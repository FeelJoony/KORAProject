#include "KORACustomEditorsModule.h"
#include "DebugHeader.h"
#include "Modules/ModuleManager.h"
#include "Core/DirectoryChangeWatcher.h"
#include "DataTool/CSVToDataTableToolSubsystem.h"
#include "DataTool/DataTableToolSettings.h"
#include "Slate/ConstructMonsterWidget.h"


//#include "Widgets/Notifications/SNotificationList.h"
//#include "Framework/Notifications/NotificationManager.h"
#include "Slate/ConstructMonsterWindow.h"
#include "Styling/AppStyle.h"
#include "SubSystem/KRDataTablesSubsystem.h"

#define LOCTEXT_NAMESPACE "FPlayfabTestEditorModule"

IMPLEMENT_MODULE(FKORACustomEditors, KORACustomEditors);

void FKORACustomEditors::StartupModule()
{
	ToolMenuExtensibilityManagerDelegateHandle = UToolMenus::RegisterStartupCallback(
		FSimpleDelegate::CreateRaw(this, &FKORACustomEditors::RegisterMenus)
	);

	RegisterTabs();

	SetOnDirectoryChangeWatcherCallbacks();
	SetOnInitializedEditorCallbacks();
}

void FKORACustomEditors::ShutdownModule()
{
	UToolMenus::UnregisterOwner(TEXT("KORACustomEditorsModule"));
	
	if (ToolMenuExtensibilityManagerDelegateHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenuExtensibilityManagerDelegateHandle);
		ToolMenuExtensibilityManagerDelegateHandle.Reset();
	}

	FEditorDelegates::OnEditorInitialized.RemoveAll(this);

	Watcher.Reset();
}

void FKORACustomEditors::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(TEXT("KORACustomEditorsModule"));

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
		
		Section.AddSubMenu(
			"MyTool",
			LOCTEXT("MyToolMenuTitle", "My Tools"),
			LOCTEXT("MyToolsMenuTooltip", "Common custom editor tools for my project."),
			FNewMenuDelegate::CreateStatic(&FKORACustomEditors::RegisterMyToolsMenu)
		);
	}
}

void FKORACustomEditors::RegisterTabs()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner
	(
		FName(TEXT("ConstructMonsterTool")),
		FOnSpawnTab::CreateRaw(this, &FKORACustomEditors::OnSpawnConstructMonsterTab)
	).SetDisplayName(FText::FromString(TEXT("ConstructMonsterTool")));
}

void FKORACustomEditors::UnregisterTabs()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("ConstructMonsterTool"));
}

TSharedRef<SDockTab> FKORACustomEditors::OnSpawnConstructMonsterTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(SConstructMonsterWindow)
	];
}

void FKORACustomEditors::RegisterMyToolsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString("Convert CSV to Data Tables"),
		LOCTEXT("ConvertCSVToDataTablesTooltip", "Converts all CSV files in the specified folder to Data Tables."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ImportAsset"),
		FUIAction(FExecuteAction::CreateStatic(&FKORACustomEditors::OnConvertCSVToDataTablesClicked))
	);

	MenuBuilder.AddMenuEntry(
			FText::FromString("Construct Monster"),
			LOCTEXT("ConstructMonsterTooltip", "Construct Monster"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ImportAsset"),
			FUIAction(FExecuteAction::CreateStatic(&FKORACustomEditors::OnOpenToolWindowThatConstructMonster))
	);
}

void FKORACustomEditors::OnConvertCSVToDataTablesClicked()
{
	EAppReturnType::Type DialogReturnType = FDebugHeader::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("CSV 파일이 변경 되었습니다. 업데이트 하시겠습니까?"));
	if (DialogReturnType == EAppReturnType::Cancel)
	{
		return;
	}

	UCSVToDataTableToolSubsystem* Subsystem = GEditor->GetEditorSubsystem<UCSVToDataTableToolSubsystem>();
	if (Subsystem)
	{
		Subsystem->ConvertAllCSVsToDataTables();
	}
	else
	{
		FNotificationInfo Info(LOCTEXT("SubsystemNotFound", "CSV to DataTable Tool: Subsystem not found or not initialized!"));
		Info.ExpireDuration = 5.f;
		Info.Image = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Notification.Fail").GetIcon();
		FSlateNotificationManager::Get().AddNotification(Info);
		UE_LOG(LogTemp, Error, TEXT("UCSVToDataTableToolSubsystem not found or not initialized!"));
	}

	// UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(this);
	// DataTablesSubsystem.InitializeDataTables();
}

void FKORACustomEditors::OnOpenToolWindowThatConstructMonster()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("ConstructMonsterTool"));
}

void FKORACustomEditors::OnConvertCSVToDataTablesClicked(double EditorLoadTime)
{
	OnConvertCSVToDataTablesClicked();
}

void FKORACustomEditors::SetOnDirectoryChangeWatcherCallbacks()
{
	if (false == Watcher.IsValid())
	{
		Watcher = MakeUnique<FDirectoryChangeWatcher>();
	}
	Watcher->StartWatchingDirectory(TEXT("Data/CSV"), FDirectoryChangeDelegate::CreateLambda([&]() {
		OnConvertCSVToDataTablesClicked();
	}));
}

void FKORACustomEditors::SetOnInitializedEditorCallbacks()
{
	FEditorDelegates::OnEditorInitialized.AddRaw(this, &FKORACustomEditors::OnConvertCSVToDataTablesClicked);
}
