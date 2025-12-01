#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_EquippableItem.generated.h"

class UKREquipmentDefinition;

UCLASS()
class KORAPROJECT_API UInventoryFragment_EquippableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	// 이 아이템을 장착했을 때 사용할 장비 정의(Equipment Definition)입니다.
	// 여기에 외형, 능력치 등 모든 장비 관련 정보가 연결됩니다.
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UKREquipmentDefinition> EquipmentDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UAnimInstance> EquippableAnimLayer;
	
	FORCEINLINE class UKREquipmentInstance* GetEquipInstance() const
	{
		return EquipInstance;
	}

	FORCEINLINE void SetEquipInstance(class UKREquipmentInstance* NewInstance)
	{
		this->EquipInstance = NewInstance;
	}

	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Fragment.Item.Equippable"); }

	
	
private:
	UPROPERTY()
	TObjectPtr<class UKREquipmentInstance> EquipInstance;

	
};
