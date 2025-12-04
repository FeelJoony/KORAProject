#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_QuickSlot.generated.h"

UCLASS(BlueprintType, Blueprintable)
class KORAPROJECT_API UInventoryFragment_QuickSlot : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Ability.Item.QuickSlot");
	}
	
	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;
};
