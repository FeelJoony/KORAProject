#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_ModuleItem.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_ModuleItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

	// UPROPERTY(EditDefaultsOnly)
	// TArray<FModuleStatData> ModuleData;
	
public:
	void ApplyToStats(class UInventoryFragment_SetStats* Stats);
	void RemoveFromStats(class UInventoryFragment_SetStats* Stats);

	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Module");
	}
};
