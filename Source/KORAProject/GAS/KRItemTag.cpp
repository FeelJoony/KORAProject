#include "GAS/KRItemTag.h"

// ----- Ability.Item -----
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Item_Stackable, "Ability.Item.Stackable", "Stackable item");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Item_Buyable, "Ability.Item.Buyable", "Buyable item");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Item_Sellable, "Ability.Item.Sellable", "Sellable item");

// ----- Ability.Consume -----
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Consume_Heal, "Ability.Consume.Heal", "소비 효과: HP 회복");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Consume_Stamina, "Ability.Consume.Stamina", "소비 효과: 스태미나 회복");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Consume_CoreCharge, "Ability.Consume.CoreCharge", "소비 효과: 카탈리스트 충전");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Consume_ATKBuff, "Ability.Consume.ATKBuff", "소비, 버프: 공격력 증가");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Consume_Stealth, "Ability.Consume.Stealth", "소비, 버프: 탐지반경 줄어듦");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Consume_Light, "Ability.Consume.Light", "광원 가능");

// ----- Ability.Equip / Unequip -----
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Equip_Sword, "Ability.Equip.Sword", "검 장착");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Equip_Gun, "Ability.Equip.Gun", "총 장착");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Unequip_Sword, "Ability.Unequip.Sword", "검 해제");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Unequip_Gun, "Ability.Unequip.Gun", "총 해제");
