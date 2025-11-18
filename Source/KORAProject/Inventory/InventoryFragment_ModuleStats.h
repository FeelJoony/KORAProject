#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_WeaponStats.h"
#include "InventoryFragment_ModuleStats.generated.h"

// 나중에 DT 상의 후 추가할지 안할지 결정
USTRUCT(BlueprintType)
struct FModuleStatModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	float AddCritMultiplier = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float AddAttackSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float AddAccuracy = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float RecoilReduce = 0.f;

	UPROPERTY(EditDefaultsOnly)
	int32 AddCapacity = 0;
};

UCLASS()
class KORAPROJECT_API UInventoryFragment_ModuleStats : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Module")
	FModuleStatModifier Modifier;
	
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Module");
	}

	// 사실상 이 부분도 StatTag가 들어간다면 바뀔 내용임(그냥 Stats 관련 코드가 사라질수도 있음)
	void ApplyToWeapon(class UInventoryFragment_WeaponStats* WeaponStats);
	void RemoveFromWeapon(class UInventoryFragment_WeaponStats* WeaponStats);
};
