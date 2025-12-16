#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "GAS/AbilitySet/KRAbilitySet.h"
#include "InventoryFragment_EquippableItem.generated.h"

class UInputMappingContext;
class UKREquipmentDefinition;

UCLASS()
class KORAPROJECT_API UInventoryFragment_EquippableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) override;
	
	FORCEINLINE class UKREquipmentInstance* GetEquipInstance() const
	{
		return EquipInstance;
	}

	FORCEINLINE void SetEquipInstance(class UKREquipmentInstance* NewInstance)
	{
		this->EquipInstance = NewInstance;
	}

	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Fragment.Item.Equippable"); }

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<class UKREquipmentDefinition> EquipmentDefinition;
	
	void LoadFromDataTable(UKRInventoryItemInstance* Instance);
	
private:
	UPROPERTY()
	TObjectPtr<class UKREquipmentInstance> EquipInstance;

	
};
