// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "KRItemUIData.h"
#include "KRUIAdapterLibrary.generated.h"

// class UKRBaseItem;

UCLASS()
class KORAPROJECT_API UKRUIAdapterLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    //UFUNCTION(BlueprintCallable, Category = "KR|UI")
    //static FKRItemUIData MakeUIDataFromItem(UKRBaseItem* Item);

    //UFUNCTION(BlueprintCallable, Category = "KR|UI")
    //static void ConvertItemsToUIData(const TArray<UKRBaseItem*>& In, TArray<FKRItemUIData>& Out);
};
