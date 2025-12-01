#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_CostumeItem.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_CostumeItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CostumeID = -1;

	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;
	
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Material");
	}
};
