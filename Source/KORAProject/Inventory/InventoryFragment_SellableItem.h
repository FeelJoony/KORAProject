#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_SellableItem.generated.h"
 
UCLASS()
class KORAPROJECT_API UKRInventoryFragment_SellableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	bool bCanBeSold = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bCanBeSold"))
	int32 SellPrice = 0;

public:
	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Ability.Item.Sellable"); }
};