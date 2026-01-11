// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "Internationalization/StringTable.h"
#include "UObject/SoftObjectPath.h"
#include "KRItemDescriptionBase.generated.h"

class UTexture2D;

UCLASS()
class KORAPROJECT_API UKRItemDescriptionBase : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	TSoftObjectPtr<UStringTable> ItemStringTableAsset = TSoftObjectPtr<UStringTable>(FSoftObjectPath(TEXT("/Game/UI/StringTable/ST_ItemList.ST_ItemList")));

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateItemInfo(const FName& ItemNameKey, const FName& ItemDescriptionKey, const TSoftObjectPtr<UTexture2D>& ItemIcon, int32 UpgradeLevel = -1);

	UFUNCTION(BlueprintCallable, Category = "Item Description")
	FText GetLocalizedTextFromKey(const FName& InKey);

protected:
	virtual void NativeConstruct() override;

private:
	void EnsureStringTableLoaded();

	UPROPERTY(Transient)
	TObjectPtr<UStringTable> CachedStringTable;

	bool bStringTableLoaded = false;
};
