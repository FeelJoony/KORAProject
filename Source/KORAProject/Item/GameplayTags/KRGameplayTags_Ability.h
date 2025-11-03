#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FAbilityGameplayTags
{
    static const FAbilityGameplayTags& Get();
    static void InitializeNativeTags();

    // ===== Ability.Elemental =====
    FGameplayTag Elemental_Fire;
    FGameplayTag Elemental_Ice;
    FGameplayTag Elemental_Electronic;
    FGameplayTag Elemental_Dark;

    // ===== Ability.ApplyCC =====
    FGameplayTag ApplyCC_Stun;
    FGameplayTag ApplyCC_Slow;
    FGameplayTag ApplyCC_Groggy;

    // ===== Ability.Item =====
    FGameplayTag Item_Stackable;
    FGameplayTag Item_Buyable;
    FGameplayTag Item_Sellable;

    // ===== Ability.Consume =====
    FGameplayTag Consume_Heal;
    FGameplayTag Consume_Stamina;
    FGameplayTag Consume_CoreCharge;
    FGameplayTag Consume_ATKBuff;
    FGameplayTag Consume_Stealth;

    // ===== Ability.Tool =====
    FGameplayTag Tool_Light;

private:
    static FAbilityGameplayTags Tags;
};
