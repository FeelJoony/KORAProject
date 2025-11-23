#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_EnhanceableItem.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_EnhanceableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

	// UPROPERTY(EditDefaultsOnly)
	// TArray<FWeaponReforgeData> ReforgeData;
	
public:
	UPROPERTY(EditDefaultsOnly)
	int32 EnhanceLevel = 0;

	// 이 친구는 DT에서 받아올 친구 임시로 이렇게 해둠
	UPROPERTY(EditDefaultsOnly)
	int32 BaseEnhanceCost = 300;
	
	int32 GetNextLevelCost() const
	{
		return BaseEnhanceCost * (EnhanceLevel + 1);
	}

	void IncreaseLevel()
	{
		EnhanceLevel++;
	}

	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Enhanceable");
	}
};
