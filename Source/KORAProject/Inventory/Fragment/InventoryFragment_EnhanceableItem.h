#pragma once

#include "CoreMinimal.h"
#include "Data/WeaponEnhanceDataStruct.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_EnhanceableItem.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_EnhanceableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	int32 EnhanceLevel = 0;
	int32 EnhanceCost = 0;
	// Weapon Type Tag!!!
	FGameplayTag WeaponTypeTag;

	void IncreaseLevel()
	{
		EnhanceLevel++;
	}

	static FGameplayTag GetStaticFragmentTag();
	
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Enhanceable");
	}

	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) override;
};
