#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Data/EquipAbilityDataStruct.h"
#include "InventoryFragment_ModuleItem.generated.h"

struct FModuleDataStruct;

UCLASS()
class KORAPROJECT_API UInventoryFragment_ModuleItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;

	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Module");
	}

	UFUNCTION(BlueprintCallable, Category = "Module")
	const TArray<FKREquipAbilityModifierRow>& GetStatModifiers() const { return CachedModifiers; }
	
	UFUNCTION(BlueprintCallable, Category = "Module")
	FGameplayTag GetTargetWeaponSlotTag() const { return TargetWeaponSlotTag; }
	
	UFUNCTION(BlueprintCallable, Category = "Module")
	bool IsInitialized() const { return bIsInitialized; }

protected:
	void InitializeFromData(class UKRInventoryItemInstance* Instance);

private:
	UPROPERTY() FGameplayTag TargetWeaponSlotTag;
	UPROPERTY() TArray<FKREquipAbilityModifierRow> CachedModifiers;
	
	bool bIsInitialized = false;
};
