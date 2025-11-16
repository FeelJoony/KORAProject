// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "KRItemDescriptionBase.generated.h"

class UTexture2D;

UCLASS()
class KORAPROJECT_API UKRItemDescriptionBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateItemInfo(const FName& ItemNameKey, const FName& ItemDescriptionKey, const TSoftObjectPtr<UTexture2D>& ItemIcon, int32 UpgradeLevel = -1);
};
