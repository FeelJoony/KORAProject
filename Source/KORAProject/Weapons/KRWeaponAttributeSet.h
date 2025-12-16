#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "KRWeaponAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 무기 관련 Attribute Set
 * WeaponInstance의 스펙을 AttributeSet에 반영
 */
UCLASS()
class KORAPROJECT_API UKRWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
/*
public:
	//UKRWeaponAttributeSet();

	// 무기 공격력
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	FGameplayAttributeData WeaponAttackPower;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, WeaponAttackPower)

	// 크리티컬 확률
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, CriticalChance)

	// 크리티컬 배율
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	FGameplayAttributeData CriticalMultiplier;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, CriticalMultiplier)

	// 공격 속도
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, AttackSpeed)
	*/
};