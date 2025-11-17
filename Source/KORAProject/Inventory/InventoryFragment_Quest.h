// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_Quest.generated.h"

/**
 * 
 */
UCLASS()
class KORAPROJECT_API UInventoryFragment_Quest : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 MaterialID = -1;

	virtual FGameplayTag GetFragmentTag() const override 
	{
		return FGameplayTag::RequestGameplayTag("Item.Fragment.Material");
	}
};
