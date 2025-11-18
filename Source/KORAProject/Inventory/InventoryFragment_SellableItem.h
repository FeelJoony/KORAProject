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
	int32 BasePrice = 0;
	
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Ability.Item.Sellable");
	}

	FORCEINLINE int32 GetBasePrice() const
	{
		return BasePrice;
	}
};