#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_QuestItem.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_QuestItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 QuestID = -1;

	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;
	
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Quest");
	}
};
