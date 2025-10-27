#include "KORACustomEditorsModule.h"
#include "Modules/ModuleManager.h"
#include "DataTool/CSVToDataTableToolSubsystem.h"
#include "DataTool/DataTableToolSettings.h"

#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "FPlayfabTestEditorModule"

IMPLEMENT_MODULE(FKORACustomEditors, KORACustomEditors);

void FKORACustomEditors::StartupModule()
{
	ToolMenuExtensibilityManagerDelegateHandle = UToolMenus::RegisterStartupCallback(
		FSimpleDelegate::CreateRaw(this, &FKORACustomEditors::RegisterMenus)
	);
}

void FKORACustomEditors::ShutdownModule()
{
	UToolMenus::UnregisterOwner(TEXT("KORACustomEditorsModule"));

	if (ToolMenuExtensibilityManagerDelegateHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenuExtensibilityManagerDelegateHandle);
		ToolMenuExtensibilityManagerDelegateHandle.Reset();
	}
}

void FKORACustomEditors::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(TEXT("KORACustomEditorsModule"));

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
		Section.AddSubMenu(
			"DataTool",
			LOCTEXT("MyToolMenuTitle", "Data Tools"),
			LOCTEXT("MyToolsMenuTooltip", "Common custom editor tools for my project."),
			FNewMenuDelegate::CreateStatic(&FKORACustomEditors::RegisterMyToolsMenu)
		);
	}
}

void FKORACustomEditors::RegisterMyToolsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString("Convert CSV to Data Tables"),
		LOCTEXT("ConvertCSVToDataTablesTooltip", "Converts all CSV files in the specified folder to Data Tables."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ImportAsset"),
		FUIAction(FExecuteAction::CreateStatic(&FKORACustomEditors::OnConvertCSVToDataTablesClicked))
	);
}

void FKORACustomEditors::OnConvertCSVToDataTablesClicked()
{
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
}
