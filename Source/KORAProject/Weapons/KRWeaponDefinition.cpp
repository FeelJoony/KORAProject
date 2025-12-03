#include "KRWeaponDefinition.h"

#include "KRWeaponInstance.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Weapons/KRMeleeWeaponInstance.h"
#include "Weapons/KRRangeWeaponInstance.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "Data/WeaponDataStruct.h"
#include "Data/WeaponEnhanceDataStruct.h"

UKRWeaponDefinition::UKRWeaponDefinition()
{
}

UKRInventoryItemDefinition* UKRWeaponDefinition::CreateInventoryItemDefinition(const FWeaponDataStruct& WeaponData, const FWeaponEnhanceDataStruct& WeaponEnhanceData)
{
    UE_LOG(LogTemp, Warning, TEXT("[InventoryDefTest] ATK=%d CritChance=%.2f Range=%d Speed=%.2f"),
        WeaponData.Atk,
        WeaponData.CritChance,
        WeaponData.Range,
        WeaponData.AttackSpeed
    );
    
    AutoAssignAllClasses();
    
    UKRInventoryItemDefinition* ItemDef = NewObject<UKRInventoryItemDefinition>(this);

    // 1. 장착 Fragment 설정
    UInventoryFragment_EquippableItem* EquipFragment = NewObject<UInventoryFragment_EquippableItem>(ItemDef);
    EquipFragment->EquipmentDefinition = EquipmentDefinition;
    EquipFragment->EquippableAnimLayer = EquippableAnimLayer;
    EquipFragment->WeaponActorToSpawn = WeaponActorClass;
    EquipFragment->AttachSocketName = AttachSocketName;
    EquipFragment->AttachTransform = AttachTransform;
    EquipFragment->WeaponIMC = WeaponIMC;
    EquipFragment->InputPriority = InputPriority;
    EquipFragment->GrantedAbilities = GrantedAbilities;
    ItemDef->AddFragment(EquipFragment->GetFragmentTag(), EquipFragment);

    // 2. 스탯 Fragment 설정
    UInventoryFragment_SetStats* StatsFragment = NewObject<UInventoryFragment_SetStats>(ItemDef);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.ATK"), WeaponData.Atk);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritChance"), WeaponData.CritChance);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritMulti"), WeaponData.CritMulti);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.Range"), WeaponData.Range);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.AttackSpeed"), WeaponData.AttackSpeed);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.Capacity"), WeaponData.Capacity); // Range 전용
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.ReloadTime"), WeaponData.ReloadTime); // Range 전용
    ItemDef->AddFragment(StatsFragment->GetFragmentTag(), StatsFragment);

    // 3. 강화 Fragment 설정
    UInventoryFragment_EnhanceableItem* EnhanceFragment = NewObject<UInventoryFragment_EnhanceableItem>(ItemDef);
    EnhanceFragment->EnhanceLevel = 0;
    EnhanceFragment->EnhanceCost = WeaponEnhanceData.Cost;
    EnhanceFragment->WeaponTypeTag = WeaponTypeTag;
    ItemDef->AddFragment(EnhanceFragment->GetFragmentTag(), EnhanceFragment);

    return ItemDef;
}

void UKRWeaponDefinition::AutoAssignAllClasses()
{
    // 이미 수동 설정되어 있으면 건너뜀
    if (WeaponInstanceClass != nullptr && WeaponActorClass != nullptr)
        return;

    // 무기 타입 기반 매핑
    const FGameplayTag MeleeTag = FGameplayTag::RequestGameplayTag("ItemType.Equip.Sword");
    const FGameplayTag RangeTag = FGameplayTag::RequestGameplayTag("ItemType.Equip.Gun");

    if (WeaponTypeTag.MatchesTag(MeleeTag))
    {
        WeaponInstanceClass = UKRMeleeWeaponInstance::StaticClass();
        WeaponActorClass    = AKRMeleeWeapon::StaticClass();
    }
    else if (WeaponTypeTag.MatchesTag(RangeTag))
    {
        WeaponInstanceClass = UKRRangeWeaponInstance::StaticClass();
        WeaponActorClass    = AKRRangeWeapon::StaticClass();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("KRWeaponDefinition: Unknown WeaponTypeTag [%s]"), *WeaponTypeTag.ToString());
    }
}
