#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_ConsumeAbility.generated.h"

class UAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API UInventoryFragment_ConsumeAbility : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityID = -1; // DT 기반

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	FGameplayTag ConsumableTypeTag;

public:
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Item.Fragment.Consumable.Ability");
	}

	virtual void ApplyToASC(class UAbilitySystemComponent* ASC)
	{
		// TODO: AbilityID 기반 AbilitySpec 생성 후 ASC->GiveAbility()
		// 인스턴스, 버프, 설치, 자동 사용(사망 시), 상호작용 
		if (!ASC) return;
	}

	// 사용 끝나는 ASC 도 고민 중
};
