#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "KREnemyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class KORAPROJECT_API UKREnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UKREnemyAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Enemy")
	FGameplayAttributeData CurrentPoise;
	ATTRIBUTE_ACCESSORS(UKREnemyAttributeSet, CurrentPoise)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Enemy")
	FGameplayAttributeData MaxPoise;
	ATTRIBUTE_ACCESSORS(UKREnemyAttributeSet, MaxPoise)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Enemy")
	FGameplayAttributeData PoiseRecoveryRate;
	ATTRIBUTE_ACCESSORS(UKREnemyAttributeSet, PoiseRecoveryRate)
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Enemy")
	FGameplayAttributeData PoiseRecoveryDelay;
	ATTRIBUTE_ACCESSORS(UKREnemyAttributeSet, PoiseRecoveryDelay)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Enemy")
	FGameplayAttributeData CanAttackRange;
	ATTRIBUTE_ACCESSORS(UKREnemyAttributeSet, CanAttackRange)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Enemy")
	FGameplayAttributeData EnterRageStatusRate;
	ATTRIBUTE_ACCESSORS(UKREnemyAttributeSet, EnterRageStatusRate)
	
};
