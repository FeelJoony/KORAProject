#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_WeaponStats.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_WeaponStats : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float BaseATK = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float CritChance = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float CritMultiplier = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float Range = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float Accuracy = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float Recoil = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	int32 Capacity = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float ReloadTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	float AttackSpeed = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	int32 ReforgeLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponStats")
	int32 BaseReforgeCost = 0;
	
	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) override
	{
		// TODO: 나중에 ASC 적용
	}
	
	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Ability.Item.WeaponStats");
	}

	void IncreaseReforgeLevel();
	
	void ApplyReforgeStats(const struct FWeaponReforgeRow& Row);

	// 이 부분 StatTag를 사용하여서 간결화 가능하지만 이 부분은 태그 + 구조 상의가 필요함 (StatTag 구조로 가면 SetStats의 데이터 Add 방식과 유사함)
	void ApplyModifier(float AddCritMultiplier, float AddAttackSpeed, float AddAccuracy, float ReduceRecoil, int32 AddCapacity);
	void RemoveModifier(float AddCritMultiplier, float AddAttackSpeed, float AddAccuracy, float ReduceRecoil, int32 AddCapacity);
};
