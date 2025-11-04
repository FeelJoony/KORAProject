#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "ToolMenus.h"

class FKORACustomEditors : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();

	static void RegisterMyToolsMenu(FMenuBuilder& MenuBuilder);
	static void OnConvertCSVToDataTablesClicked();
	void OnConvertCSVToDataTablesClicked(double EditorLoadTime);

	void SetOnDirectoryChangeWatcherCallbacks();
	void SetOnInitializedEditorCallbacks();

	TUniquePtr<class FDirectoryChangeWatcher> Watcher;

	FDelegateHandle ToolMenuExtensibilityManagerDelegateHandle;
};
