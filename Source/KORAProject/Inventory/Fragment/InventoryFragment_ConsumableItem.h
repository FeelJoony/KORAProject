#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_ConsumableItem.generated.h"

class UAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API UInventoryFragment_ConsumableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable")
	int32 ConsumeID = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Consumable")
	FGameplayTagContainer AbilityTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable")
	float Power = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable")
	float Duration = 0.f;
	
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Consumable");
	}

	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;
};
