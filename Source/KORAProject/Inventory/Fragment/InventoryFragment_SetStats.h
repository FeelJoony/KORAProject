#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Equipment/KRWeaponStatsCache.h"
#include "InventoryFragment_SetStats.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_SetStats : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;
	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Ability.Item.SetStat"); }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	const FWeaponStatsCache& GetWeaponStats() const { return CachedStats; }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsInitialized() const { return bIsInitialized; }

protected:
	void InitializeWeaponStats(class UKRInventoryItemInstance* Instance);
	
private:
	UPROPERTY() FWeaponStatsCache CachedStats;
	bool bIsInitialized = false;
};
