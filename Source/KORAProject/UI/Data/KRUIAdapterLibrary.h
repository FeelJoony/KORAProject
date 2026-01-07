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

    UFUNCTION(BlueprintCallable, Category = "KR|UI|Inventory", meta = (WorldContext = "WorldContextObject"))
    static void GetOwnerInventoryUIData(UObject* WorldContextObject, TArray<FKRItemUIData>& Out);

	UFUNCTION(BlueprintCallable, Category = "KR|UI|Inventory", meta = (WorldContext = "WorldContextObject"))
	static void GetInventoryUIDataFiltered(UObject* WorldContextObject, const FGameplayTag& FilterTag, TArray<FKRItemUIData>& Out);

	UFUNCTION(BlueprintCallable, Category = "KR|UI|Shop", meta = (WorldContext = "WorldContextObject"))
	static void GetShopUIData(UObject* WorldContextObject, TArray<FKRItemUIData>& Out);

	UFUNCTION(BlueprintCallable, Category = "UIAdapter|Equipment", meta = (WorldContext = "WorldContextObject"))
	static void GetEquippedCategoryUIData(UObject* WorldContextObject, const TArray<FGameplayTag>& SlotTagOrder, TArray<FKRItemUIData>& Out);
	
    UFUNCTION(BlueprintCallable, Category = "UIAdapter|Equipment", meta = (WorldContext = "WorldContextObject"))
    static bool GetEquippedSlotUIData(UObject* WorldContextObject, const FGameplayTag& SlotTag, FKRItemUIData& Out);

	UFUNCTION(BlueprintCallable, Category = "UIAdapter|QuickSlot", meta = (WorldContext = "WorldContextObject"))
	static bool GetQuickSlotUIData(UObject* WorldContextObject, const FGameplayTag& SlotTag, FKRItemUIData& Out);
};
