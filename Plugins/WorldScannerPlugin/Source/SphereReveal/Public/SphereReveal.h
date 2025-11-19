// Copyright (c) 2025 Studio3F. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSphereRevealModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
