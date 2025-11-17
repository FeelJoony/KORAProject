// Copyright (c) 2025 Studio3F. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ScanFunctionLibrary.generated.h"

class AScannerController;

/**
 * 
 */
UCLASS()
class SPHEREREVEAL_API UScanFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// World에서 첫 번째 SphereReveal ScannerController를 찾아 반환. 
	UFUNCTION(BlueprintCallable, Category="SphereReveal|Access", meta=(WorldContext="WorldContextObject"))
	static AScannerController* GetSphereRevealController(UObject* WorldContextObject);
};
