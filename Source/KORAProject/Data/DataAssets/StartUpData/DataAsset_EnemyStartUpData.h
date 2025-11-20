#pragma once

#include "CoreMinimal.h"
#include "Data//DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartUpData.generated.h"

class UKREnemyGameplayAbility;

UCLASS()
class KORAPROJECT_API UDataAsset_EnemyStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UKRAbilitySystemComponent* InASC, int32 ApplyLevel = 1) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UKREnemyGameplayAbility>> EnemyCombatAbilities;
};
