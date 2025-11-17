// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_Module_Sword.generated.h"

/**
 * 
 */
UCLASS()
class KORAPROJECT_API UInventoryFragment_Module_Sword : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Module")
	double CritDamageBonus = 0;

	UPROPERTY(EditDefaultsOnly, Category="Module")
	double AttackSpeedBonus = 0;

	/** ASC 적용 */
	void ApplyModuleToASC(/*UAbilitySystemComponent* ASC*/)
	{
		// TODO: 검 모듈 효과 ASC에 적용
	}

	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Item.Fragment.Module.Sword");
	}
};
