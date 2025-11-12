#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_EquippableItem.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_EquippableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	FORCEINLINE class UKREquipmentInstance* GetEquipInstance() const
	{
		return EquipInstance;
	}

	FORCEINLINE void SetEquipInstance(class UKREquipmentInstance* NewInstance)
	{
		this->EquipInstance = NewInstance;
	}

	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Item.Ability.Equippable"); }
	
private:
	UPROPERTY()
	TObjectPtr<class UKREquipmentInstance> EquipInstance;
	
};
