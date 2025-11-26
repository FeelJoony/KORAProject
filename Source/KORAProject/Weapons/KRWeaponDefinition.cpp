#include "KRWeaponDefinition.h"

#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "Data/WeaponDataStruct.h"
#include "Data/WeaponEnhanceDataStruct.h"

UKRInventoryItemDefinition* UKRWeaponDefinition::CreateInventoryItemDefinition(const FWeaponDataStruct& WeaponData, const FWeaponEnhanceDataStruct& WeaponEnhanceData)
{
    UKRInventoryItemDefinition* ItemDef = NewObject<UKRInventoryItemDefinition>(this);

    // 2. Equippable Fragment 추가
    UInventoryFragment_EquippableItem* EquipFragment = NewObject<UInventoryFragment_EquippableItem>(ItemDef);
    EquipFragment->EquipmentDefinition = EquipmentDefinition;
    ItemDef->AddFragment(EquipFragment->GetFragmentTag(), EquipFragment);

    // 3. SetStats Fragment 추가 (무기 스펙)
    UInventoryFragment_SetStats* StatsFragment = NewObject<UInventoryFragment_SetStats>(ItemDef);

    // DataTable의 스펙을 GameplayTag로 매핑
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.ATK"), WeaponData.Atk);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritChance"), WeaponData.CritChance);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritMulti"), WeaponData.CritMulti);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.Range"), WeaponData.Range);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.Accuracy"), WeaponData.Accuracy);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.Recoil"), WeaponData.Recoil);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.Capacity"), WeaponData.Capacity);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.ReloadTime"), WeaponData.ReloadTime);
    StatsFragment->InitStat(FGameplayTag::RequestGameplayTag("Weapon.Stat.AttackSpeed"), WeaponData.AttackSpeed);

    ItemDef->AddFragment(StatsFragment->GetFragmentTag(), StatsFragment);

    // 4. Enhanceable Fragment 추가 (강화 시스템)
    UInventoryFragment_EnhanceableItem* EnhanceFragment = NewObject<UInventoryFragment_EnhanceableItem>(ItemDef);
    EnhanceFragment->BaseEnhanceCost = WeaponEnhanceData.Cost;
    EnhanceFragment->EnhanceLevel = 0;
    ItemDef->AddFragment(EnhanceFragment->GetFragmentTag(), EnhanceFragment);

    return ItemDef;
}