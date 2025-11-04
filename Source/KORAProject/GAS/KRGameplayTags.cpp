#include "GAS/KRGameplayTags.h"

namespace KRGameplayTags
{
	// ----- ItemType -----
	UE_DEFINE_GAMEPLAY_TAG(TAG_ItemType, "ItemType");

	// ----- ItemType.Equip -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip, "ItemType.Equip", "장비 아이템");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip_Sword, "ItemType.Equip.Sword", "장비 - 검");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip_Gun, "ItemType.Equip.Gun", "장비 - 총");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip_GunModule, "ItemType.Equip.GunModule", "장비 - 총 모듈");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip_SwordModule, "ItemType.Equip.SwordModule", "장비 - 검 모듈");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip_Armor, "ItemType.Equip.Armor", "장비 - 갑옷 (아머)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip_Head, "ItemType.Equip.Head", "장비 - 머리");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Equip_Costume, "ItemType.Equip.Costume", "장비 - 코스튬");

	// ----- ItemType.Consume -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Consume, "ItemType.Consume", "소모품 아이템");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Consume_Potion, "ItemType.Consume.Potion", "소모품 - 물약");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Consume_InsuranceCoin, "ItemType.Consume.Insurance.Coin", "소모품 - 보험류 코인");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Consume_InsuranceWallet, "ItemType.Consume.Insurance.Wallet", "소모품 - 보험류 지갑");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Consume_Key, "ItemType.Consume.Key", "소모품 - 키");

	// ----- ItemType.Tool -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Tool, "ItemType.Tool", "도구 아이템");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Tool_Light, "ItemType.Tool.Light", "도구 - 광원");

	// ----- ItemType.Material -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Material, "ItemType.Material", "재료 아이템");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Material_Scrap, "ItemType.Material.Scrap", "재료 - 스크랩");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Material_Tech, "ItemType.Material.Tech", "재료 - 테크 재료");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Material_Doc, "ItemType.Material.Doc", "재료 - 문서");
	
	// ----- ItemType.Quest -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ItemType_Quest, "ItemType.Quest", "퀘스트 아이템");

	// ----- Weapon -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Weapon, "Weapon", "무기 효과/속성");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Weapon_Gun_CritMultiplier, "Weapon.Gun.CritMultiplier", "무기 모듈 효과 - 치명타 배율 (총)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Weapon_Sword_CritMultiplier, "Weapon.Sword.CritMultiplier", "무기 모듈 효과 - 치명타 배율 (검)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Weapon_Both_AttackSpeed, "Weapon.Both.AttackSpeed", "무기 모듈 효과 - 공격 속도 (공용)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Weapon_Gun_Accuracy, "Weapon.Gun.Accuracy", "무기 모듈 효과 - 명중률 (총)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Weapon_Gun_Recoil, "Weapon.Gun.Recoil", "무기 모듈 효과 - 반동 (총)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Weapon_Gun_ExtendMag, "Weapon.Gun.ExtendMag", "무기 모듈 효과 - 확장 탄창 (총)");

	// ----- Ability -----

	// ----- Item Property -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Item_Property, "Item.Property", "아이템 속성");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Item_Property_Stackable, "Item.Property.Stackable", "아이템 스택 가능 여부");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Item_Property_Buyable, "Item.Property.Buyable", "구매 가능 여부");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Item_Property_Sellable, "Item.Property.Sellable", "판매 가능 여부");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Item_Property_Unsellable, "Item.Property.Unsellable", "판매 불가 여부");

	// ----- Item Consume Effect ----

	// ----- Item Tool Effect ----

	// ----- Equipment Slot -----
}