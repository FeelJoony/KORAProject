// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "KRItemUIData.h"
#include "KRUIAdapterLibrary.generated.h"

class UKRInventoryItemInstance;

UCLASS()
class KORAPROJECT_API UKRUIAdapterLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "KR|UI")
    static bool MakeUIDataFromItemInstance(const UKRInventoryItemInstance* Instance, FKRItemUIData& Out);

    UFUNCTION(BlueprintCallable, Category = "KR|UI")
    static void ConvertItemInstancesToUIData(const TArray<UKRInventoryItemInstance*>& In, TArray<FKRItemUIData>& Out);

    UFUNCTION(BlueprintCallable, Category = "KR|UI", meta = (WorldContext = "WorldContextObject"))
    static void GetOwnerInventoryUIData(UObject* WorldContextObject, TArray<FKRItemUIData>& Out);
};
