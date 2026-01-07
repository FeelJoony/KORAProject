#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "EnemyAbilityDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FEnemyAbilityDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAbility)
	FGameplayTag EnemyTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAbility)
	TSoftObjectPtr<class UKRGameplayAbility> AttackGA;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAbility)
	TSoftObjectPtr<class UKRGameplayAbility> HitGA;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAbility)
	TSoftObjectPtr<class UKRGameplayAbility> DeathGA;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAbility)
	TSoftObjectPtr<class UKRGameplayAbility> StunGA;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAbility)
	TSoftObjectPtr<class UKRGameplayAbility> SlashGA;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAbility)
	TSoftObjectPtr<class UKRGameplayAbility> AlertGA;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(EnemyTag);
	}
};
