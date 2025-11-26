#pragma once

#include "CoreMinimal.h"
#include "Equipment/KREquipmentInstance.h"
#include "GameplayTagContainer.h"
#include "KRWeaponInstance.generated.h"

struct FKRWeaponItemData;
class UInventoryFragment_SetStats;

/**
 * Weapon Instance
 * 런타임에 생성되어 무기의 상태와 스펙을 관리
 */
UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKRWeaponInstance : public UKREquipmentInstance
{
    GENERATED_BODY()

public:
    UKRWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // Equipment Instance Override
    virtual void OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn) override;
    virtual void OnUnequipped() override;

    // 무기 스펙 초기화 (Fragment의 SetStats에서 가져옴)
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void InitializeStats(const UInventoryFragment_SetStats* StatsFragment) override;
    
    // 무기 타입
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    FGameplayTag WeaponType; // Weapon.Type.Sword, Weapon.Type.Gun

    // 무기 스펙 (캐시)
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float AttackPower;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float CritChance;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float CritMultiplier;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float Range;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float AttackSpeed;

    // 총 전용 스펙
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    int32 CurrentAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    int32 MaxAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    float ReloadTime;

    UPROPERTY()
    AKRWeaponBase* SpawnedWeaponActor;

    AKRWeaponBase* UKRWeaponInstance::GetSpawnedWeaponActor() const
    {
        return SpawnedWeaponActor;
    }

    virtual void SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn) override;
    
    // 데미지 계산 (GAS GameplayEffect에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
    float CalculateDamage(bool& bOutIsCritical);

    // 강화 레벨 적용
    UFUNCTION(BlueprintCallable, Category = "Weapon|Enhance")
    virtual void ApplyEnhanceLevel(int32 EnhanceLevel) override;

protected:
    // 강화 레벨에 따른 스텟 배율
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Enhance")
    float EnhanceDamageMultiplierPerLevel = 0.1f; // 레벨당 10% 증가

    float EnhanceMultiplier = 1.0f;
};