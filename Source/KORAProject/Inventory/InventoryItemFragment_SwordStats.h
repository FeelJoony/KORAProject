// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryItemFragment_SwordStats.generated.h"


UCLASS()
class KORAPROJECT_API UInventoryFragment_SwordStats : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float AttackPower = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float CritChance = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float CritDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	float AttackSpeed = 0.f;

public:
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Item.Fragment.Weapon.CommonStats");
	}

	/** 장착 시 ASC에 스탯 적용 */
	virtual void ApplyToASC(class UAbilitySystemComponent* ASC)
	{
		// TODO: ASC에 스탯 반영 (GameplayEffect 생성 or 직접 Attribute 변경)
	}

	/** 해제 시 ASC에서 스탯 제거 */
	virtual void RemoveFromASC(class UAbilitySystemComponent* ASC)
	{
		// TODO: ASC에서 스탯 제거
	}
};

