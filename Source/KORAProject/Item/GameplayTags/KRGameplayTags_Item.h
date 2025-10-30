#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FItemGameplayTags
{
    static const FItemGameplayTags& Get();
    static void InitializeNativeTags();

    /** ===== Item Types ===== */
    FGameplayTag ItemType_Equip_Sword;
    FGameplayTag ItemType_Equip_Gun;
    FGameplayTag ItemType_Equip_GunModule;
    FGameplayTag ItemType_Equip_SwordModule;
    FGameplayTag ItemType_Equip_Armor;

    FGameplayTag ItemType_Consume_Potion;
    FGameplayTag ItemType_Consume_Insurance_Coin;
    FGameplayTag ItemType_Consume_Insurance_Wallet;

    FGameplayTag ItemType_Tool_Light;

    FGameplayTag ItemType_Material_Scrap;
    FGameplayTag ItemType_Material_Tech;
    FGameplayTag ItemType_Material_Doc;

    FGameplayTag ItemType_Quest;

    /** ===== Weapon Stats ===== */
    FGameplayTag Weapon_Gun_CritMultiplier;
    FGameplayTag Weapon_Sword_CritMultiplier;
    FGameplayTag Weapon_Both_AttackSpeed;
    FGameplayTag Weapon_Gun_Accuracy;
    FGameplayTag Weapon_Gun_Recoil;
    FGameplayTag Weapon_Gun_ExtendMag;

private:
    static FItemGameplayTags Tags;
};
