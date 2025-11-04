#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FShopGameplayTags
{
    static const FShopGameplayTags& Get();
    static void InitializeNativeTags();

    // ===== Currency =====
    FGameplayTag Currency_Purchase_Gearing;
    FGameplayTag Currency_Skill_Corbyte;
    FGameplayTag Currency_Material;
    FGameplayTag Currency_Event;

    // ===== Shop.Vendor =====
    FGameplayTag Shop_Vendor_Official;
    FGameplayTag Shop_Vendor_Black;
    FGameplayTag Shop_Vendor_SoraBell;

    // ===== Shop.Stability =====
    FGameplayTag Shop_Stability_Normal;
    FGameplayTag Shop_Stability_Caution;
    FGameplayTag Shop_Stability_Emergency;

    // ===== Shop.Discount =====
    FGameplayTag Shop_Discount;

    // ===== Shop.Service =====
    FGameplayTag Shop_Service_Enhance;
    FGameplayTag Shop_Service_Repair;

    // ===== Rarity =====
    FGameplayTag Rarity_Common;
    FGameplayTag Rarity_Uncommon;
    FGameplayTag Rarity_Rare;
    FGameplayTag Rarity_Epic;

private:
    static FShopGameplayTags Tags;
};
