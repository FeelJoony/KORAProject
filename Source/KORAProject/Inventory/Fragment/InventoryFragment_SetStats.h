#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_SetStats.generated.h"

class UKRWeaponAttributeSet;

UCLASS()
class KORAPROJECT_API UInventoryFragment_SetStats : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;
	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Ability.Item.SetStat"); }

protected:
	void InitializeWeaponStats(const FGameplayTag& ItemTag);
	
private:
	UPROPERTY()
	TObjectPtr<class UKRWeaponAttributeSet> WeaponAttributeSet;

public:
	FORCEINLINE const UKRWeaponAttributeSet* GetWeaponAttributeSet() const { return WeaponAttributeSet; }
};
