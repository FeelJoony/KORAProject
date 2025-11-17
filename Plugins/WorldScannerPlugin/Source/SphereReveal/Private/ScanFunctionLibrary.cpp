// Copyright (c) 2025 Studio3F. All Rights Reserved.

#include "ScanFunctionLibrary.h"
#include "ScannerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"                       // TActorIterator

AScannerController* UScanFunctionLibrary::GetSphereRevealController(UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	for (TActorIterator<AScannerController> It(World); It; ++It)
	{
		if (AScannerController* Found = *It)
		{
			return Found; // only first one
		}
	}
	return nullptr;
}
