#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UKRGameplayAbility;
class UKRAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UKRAbilitySystemComponent* InASC, int32 ApplyLevel = 1);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UKRGameplayAbility>> ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<TSubclassOf<UKRGameplayAbility>> ReactiveAbilities;

	void GrantAbilities(const TArray<TSubclassOf<UKRGameplayAbility>>& InAbilitiesToGive, UKRAbilitySystemComponent* InASC, int32 ApplyLevel = 1);
	
};
