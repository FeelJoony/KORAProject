#pragma once

#include "CoreMinimal.h"
#include "Interface/TableKey.h"
#include "GameplayTagContainer.h"
#include "StateTree.h"
#include "EnemyDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FEnemyDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	FGameplayTag EnemyTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	TSoftObjectPtr<UStateTree> StateTree;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	TSoftObjectPtr<class UAnimMontage> AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	TSoftObjectPtr<class UAnimMontage> HitMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	TSoftObjectPtr<class UAnimMontage> DeathMontage;

	virtual uint32 GetKey() const
	{
		return GetTypeHash(EnemyTag);
	}
};
