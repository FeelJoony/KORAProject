#pragma once

#include "CoreMinimal.h"
#include "Equipment/KREquipmentInstance.h"
#include "GameplayTagContainer.h"
#include "KRWeaponInstance.generated.h"

class UInventoryFragment_SetStats;
class UInventoryFragment_EnhanceableItem;
class UKRInventoryItemInstance;

UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKRWeaponInstance : public UKREquipmentInstance
{
    GENERATED_BODY()

public:
    UKRWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // Equipment Instance Override
    virtual void OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn) override;
    virtual void OnUnequipped() override;

    void InitializeFromItem(UKRInventoryItemInstance* ItemInstance);
    // 무기 스펙 초기화 (Fragment의 SetStats에서 가져옴)
    virtual void InitializeStats(const UInventoryFragment_SetStats* StatsFragment) override;
    
    // 무기 타입
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    FGameplayTag WeaponType; // Weapon.Type.Sword, Weapon.Type.Gun

    // 무기 스펙 (캐시)
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float AttackPower;

    float BaseAttackPower;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float CritChance;

    float BaseCritChance;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float CritMultiplier;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float AttackSpeed;

    // 총 전용 스펙
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float Range;
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    int32 CurrentAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    int32 MaxAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    float ReloadTime;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentEnhanceLevel;

    UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
    float CalculateDamage(bool& bOutIsCritical);

    // 강화 레벨 적용
    virtual void ApplyEnhanceLevel(int32 EnhanceLevel) override;
};