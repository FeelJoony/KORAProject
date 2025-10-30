#include "Item/GameplayTags/KRGameplayTags_Item.h"
#include "GameplayTagsManager.h"

FItemGameplayTags FItemGameplayTags::Tags;

const FItemGameplayTags& FItemGameplayTags::Get()
{
    return Tags;
}

void FItemGameplayTags::InitializeNativeTags()
{
    auto& Manager = UGameplayTagsManager::Get();

    // ===== Item.Type =====
    Tags.ItemType_Equip_Sword = Manager.AddNativeGameplayTag("ItemType.Equip.Sword", "Sword");
    Tags.ItemType_Equip_Gun = Manager.AddNativeGameplayTag("ItemType.Equip.Gun", "Gun");
    Tags.ItemType_Equip_GunModule = Manager.AddNativeGameplayTag("ItemType.Equip.GunModule", "Gun Module");
    Tags.ItemType_Equip_SwordModule = Manager.AddNativeGameplayTag("ItemType.Equip.SwordModule", "Sword Module");
    Tags.ItemType_Equip_Armor = Manager.AddNativeGameplayTag("ItemType.Equip.Armor", "Armor");

    Tags.ItemType_Consume_Potion = Manager.AddNativeGameplayTag("ItemType.Consume.Potion", "Potion");
    Tags.ItemType_Consume_Insurance_Coin = Manager.AddNativeGameplayTag("ItemType.Consume.Insurance.Coin", "Insurance Coin");
    Tags.ItemType_Consume_Insurance_Wallet = Manager.AddNativeGameplayTag("ItemType.Consume.Insurance.Wallet", "Insurance Wallet");

    Tags.ItemType_Tool_Light = Manager.AddNativeGameplayTag("ItemType.Tool.Light", "Light");

    Tags.ItemType_Material_Scrap = Manager.AddNativeGameplayTag("ItemType.Material.Scrap", "Scrap");
    Tags.ItemType_Material_Tech = Manager.AddNativeGameplayTag("ItemType.Material.Tech", "Tech");
    Tags.ItemType_Material_Doc = Manager.AddNativeGameplayTag("ItemType.Material.Doc", "Doc");

    Tags.ItemType_Quest = Manager.AddNativeGameplayTag("ItemType.Quest", "Quest Item");

    // ===== Weapon Stats =====
    Tags.Weapon_Gun_CritMultiplier = Manager.AddNativeGameplayTag("Weapon.Gun.CritMultiplier", "Gun Crit Multiplier");
    Tags.Weapon_Sword_CritMultiplier = Manager.AddNativeGameplayTag("Weapon.Sword.CritMultiplier", "Sword Crit Multiplier");
    Tags.Weapon_Both_AttackSpeed = Manager.AddNativeGameplayTag("Weapon.Both.AttackSpeed", "Attack Speed");
    Tags.Weapon_Gun_Accuracy = Manager.AddNativeGameplayTag("Weapon.Gun.Accuracy", "Gun Accuracy");
    Tags.Weapon_Gun_Recoil = Manager.AddNativeGameplayTag("Weapon.Gun.Recoil", "Gun Recoil");
    Tags.Weapon_Gun_ExtendMag = Manager.AddNativeGameplayTag("Weapon.Gun.ExtendMag", "Extended Magazine");
}