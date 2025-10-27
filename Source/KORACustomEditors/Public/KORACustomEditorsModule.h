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

	FDelegateHandle ToolMenuExtensibilityManagerDelegateHandle;
};
