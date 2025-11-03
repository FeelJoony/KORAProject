#include "Item/GameplayTags/KRGameplayTags_Ability.h"
#include "GameplayTagsManager.h"

FAbilityGameplayTags FAbilityGameplayTags::Tags;

const FAbilityGameplayTags& FAbilityGameplayTags::Get()
{
    return Tags;
}

void FAbilityGameplayTags::InitializeNativeTags()
{
    auto& Manager = UGameplayTagsManager::Get();

    // ===== Elemental =====
    Tags.Elemental_Fire = Manager.AddNativeGameplayTag("Ability.Elemental.Fire", "Fire ability");
    Tags.Elemental_Ice = Manager.AddNativeGameplayTag("Ability.Elemental.Ice", "Ice ability");
    Tags.Elemental_Electronic = Manager.AddNativeGameplayTag("Ability.Elemental.Electronic", "Electronic ability");
    Tags.Elemental_Dark = Manager.AddNativeGameplayTag("Ability.Elemental.Dark", "Dark ability");

    // ===== ApplyCC =====
    Tags.ApplyCC_Stun = Manager.AddNativeGameplayTag("Ability.ApplyCC.Stun", "Stun CC");
    Tags.ApplyCC_Slow = Manager.AddNativeGameplayTag("Ability.ApplyCC.Slow", "Slow CC");
    Tags.ApplyCC_Groggy = Manager.AddNativeGameplayTag("Ability.ApplyCC.Groggy", "Groggy CC");

    // ===== Item =====
    Tags.Item_Stackable = Manager.AddNativeGameplayTag("Ability.Item.Stackable", "Stackable item");
    Tags.Item_Buyable = Manager.AddNativeGameplayTag("Ability.Item.Buyable", "Buyable item");
    Tags.Item_Sellable = Manager.AddNativeGameplayTag("Ability.Item.Sellable", "Sellable item");

    // ===== Consume =====
    Tags.Consume_Heal = Manager.AddNativeGameplayTag("Ability.Consume.Heal", "Recover HP");
    Tags.Consume_Stamina = Manager.AddNativeGameplayTag("Ability.Consume.Stamina", "Recover stamina");
    Tags.Consume_CoreCharge = Manager.AddNativeGameplayTag("Ability.Consume.CoreCharge", "Recover core");
    Tags.Consume_ATKBuff = Manager.AddNativeGameplayTag("Ability.Consume.ATKBuff", "Attack buff");
    Tags.Consume_Stealth = Manager.AddNativeGameplayTag("Ability.Consume.Stealth", "Stealth ability");

    // ===== Tool =====
    Tags.Tool_Light = Manager.AddNativeGameplayTag("Ability.Tool.Light", "Light tool ability");
}
