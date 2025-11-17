// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayAbilitySpec.h"
#include "InventoryFragment_ConsumeAbility.generated.h"

/**
 * 
 */
UCLASS()
class KORAPROJECT_API UInventoryFragment_ConsumeAbility : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityID = -1; // DT 기반

public:
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Item.Fragment.Consumable.Ability");
	}

	virtual void ApplyToASC(class UAbilitySystemComponent* ASC)
	{
		// TODO: AbilityID 기반 AbilitySpec 생성 후 ASC->GiveAbility()
	}
};
