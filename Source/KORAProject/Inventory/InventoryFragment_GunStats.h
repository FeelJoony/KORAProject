// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_GunStats.generated.h"

/**
 * 
 */
UCLASS()
class KORAPROJECT_API UInventoryFragment_GunStats : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float AttackPower = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float CritChance = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float CritDamage = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Range = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Accuracy = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Recoil = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Magazine = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float ReloadTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float FireRate = 0.f;

public:
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Item.Fragment.Weapon.GunStats");
	}

	virtual void ApplyToASC(class UAbilitySystemComponent* ASC)
	{
		// TODO: 총기 스탯을 ASC AttributeSet에 적용
	}

	virtual void RemoveFromASC(class UAbilitySystemComponent* ASC)
	{
		// TODO: ASC에서 전체 스탯 제거
	}
};
