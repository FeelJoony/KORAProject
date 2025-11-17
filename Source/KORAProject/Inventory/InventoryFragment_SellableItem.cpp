#include "Inventory/InventoryFragment_SellableItem.h"
#include "GameplayTagContainer.h"

FGameplayTag UKRInventoryFragment_SellableItem::GetFragmentTag() const
{
	return FGameplayTag::RequestGameplayTag("Item.Ability.Sellable");
}