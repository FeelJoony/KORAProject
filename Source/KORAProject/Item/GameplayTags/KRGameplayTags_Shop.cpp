#include "Item/GameplayTags/KRGameplayTags_Shop.h"
#include "GameplayTagsManager.h"

FShopGameplayTags FShopGameplayTags::Tags;

const FShopGameplayTags& FShopGameplayTags::Get()
{
    return Tags;
}

void FShopGameplayTags::InitializeNativeTags()
{
    auto& Manager = UGameplayTagsManager::Get();

    // ===== Currency =====
    Tags.Currency_Purchase_Gearing = Manager.AddNativeGameplayTag("Currency.Purchase.Gearing", "Gearing currency");
    Tags.Currency_Skill_Corbyte = Manager.AddNativeGameplayTag("Currency.Skill.Corbyte", "Corbyte currency");
    Tags.Currency_Material = Manager.AddNativeGameplayTag("Currency.Material", "Material currency");
    Tags.Currency_Event = Manager.AddNativeGameplayTag("Currency.Event", "Event currency");

    // ===== Shop.Vendor =====
    Tags.Shop_Vendor_Official = Manager.AddNativeGameplayTag("Shop.Vendor.Official", "Official shop");
    Tags.Shop_Vendor_Black = Manager.AddNativeGameplayTag("Shop.Vendor.Black", "Black market shop");
    Tags.Shop_Vendor_SoraBell = Manager.AddNativeGameplayTag("Shop.Vendor.SoraBell", "SoraBell shop");

    // ===== Shop.Stability =====
    Tags.Shop_Stability_Normal = Manager.AddNativeGameplayTag("Shop.Stability.Normal", "Normal stock");
    Tags.Shop_Stability_Caution = Manager.AddNativeGameplayTag("Shop.Stability.Caution", "Stock caution");
    Tags.Shop_Stability_Emergency = Manager.AddNativeGameplayTag("Shop.Stability.Emergency", "Low stock emergency");

    // ===== Shop.Discount =====
    Tags.Shop_Discount = Manager.AddNativeGameplayTag("Shop.Discount", "Shop discount");

    // ===== Shop.Service =====
    Tags.Shop_Service_Enhance = Manager.AddNativeGameplayTag("Shop.Service.Enhance", "Enhancement service");
    Tags.Shop_Service_Repair = Manager.AddNativeGameplayTag("Shop.Service.Repair", "Repair service");
}