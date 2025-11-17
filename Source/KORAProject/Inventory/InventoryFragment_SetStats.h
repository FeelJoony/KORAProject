#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_SetStats.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_SetStats : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override;

	void InitStat(FGameplayTag StatTag, double StatValue);
	void AddStat(FGameplayTag StatTag, double StatValue);
	void SubtractStat(FGameplayTag StatTag, double StatValue);
	void ClearStat(FGameplayTag StatTag);
	FORCEINLINE double GetStatByTag(FGameplayTag StatTag) const;

	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Ability.Item.SetStat"); }
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, double> Stats;

	// TODO : 우리는 나중에 GAS를 사용하기 때문에 이 AttributeSet으로 Stat을 정의해야 된다.
	// UPROPERTY(EditDefaultsOnly)
	// TObjectPtr<class UKRAttributeSet> StatAttributeSet;
};
