#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "KRCombatCommonSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class KORAPROJECT_API UKRCombatCommonSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKRCombatCommonSet();

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, CurrentHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, DefensePower)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData DealDamageMult;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, DealDamageMult)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData TakeDamageMult;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, TakeDamageMult)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, DamageTaken)
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Weapon")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, AttackSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Weapon")
	FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, CritChance)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Weapon")
	FGameplayAttributeData CritMulti;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, CritMulti)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Weapon")
	FGameplayAttributeData WeaponRange;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, WeaponRange)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Ammo")
	FGameplayAttributeData CurrentAmmo;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, CurrentAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat|Ammo")
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UKRCombatCommonSet, MaxAmmo)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
