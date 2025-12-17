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


UCLASS()
class KORAPROJECT_API UKRWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKRWeaponAttributeSet();

	// 공격 관련
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, AttackPower);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, AttackSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData Range;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, Range);

	// 치명타 관련
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, CritChance);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData CritMulti;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, CritMulti);

	// 총기 관련
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData Capacity;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, Capacity);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData ReloadTime;
	ATTRIBUTE_ACCESSORS(UKRWeaponAttributeSet, ReloadTime);
	
};
