#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Equipment/KREquipmentInstance.h"
#include "InventoryFragment_EquippableItem.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_EquippableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) override;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	class UKREquipmentInstance* GetEquipInstance() const { return EquipInstance; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsInitialized() const { return EquipInstance != nullptr && EquipInstance->IsValid(); }

	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Fragment.Item.Equippable"); }

protected:
	void InitializeEquipInstance(UKRInventoryItemInstance* Instance);

private:
	UPROPERTY()
	TObjectPtr<class UKREquipmentInstance> EquipInstance;
};
