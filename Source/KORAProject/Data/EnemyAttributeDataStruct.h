#pragma once

#include "CoreMinimal.h"
#include "Interface/TableKey.h"
#include "GameplayTagContainer.h"
#include "EnemyAttributeDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FEnemyAttributeDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	FGameplayTag EnemyTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float MaxHealth;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float AttackPower;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float DefensePower;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float DealDamageMultiply;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float TakeDamageMultiply;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float DamageTaken;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float CanAttackRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAttribute)
	float EnterRageStatusRate;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(EnemyTag);
	}
};
