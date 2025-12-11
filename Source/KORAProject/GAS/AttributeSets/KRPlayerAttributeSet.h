#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "KRPlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class KORAPROJECT_API UKRPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKRPlayerAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData CurrentStamina;
	ATTRIBUTE_ACCESSORS(UKRPlayerAttributeSet, CurrentStamina)
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UKRPlayerAttributeSet, MaxStamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData GreyHP;
	ATTRIBUTE_ACCESSORS(UKRPlayerAttributeSet, GreyHP)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UKRPlayerAttributeSet, StaminaRegenRate)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData StaminaRegenDelay;
	ATTRIBUTE_ACCESSORS(UKRPlayerAttributeSet, StaminaRegenDelay)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Playe")
	FGameplayAttributeData CoreDrive;
	ATTRIBUTE_ACCESSORS(UKRPlayerAttributeSet, CoreDrive)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData MaxCoreDrive;
	ATTRIBUTE_ACCESSORS(UKRPlayerAttributeSet, MaxCoreDrive)
};
