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

	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) override;
	
	FORCEINLINE int32 GetSellPrice() const
	{
		return BasePrice*0.8f;
	}
};