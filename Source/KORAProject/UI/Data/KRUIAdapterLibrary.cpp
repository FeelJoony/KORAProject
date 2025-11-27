// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Data/KRUIAdapterLibrary.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Subsystem/KRInventorySubsystem.h"
#include "Inventory/Fragment/InventoryFragment_DisplayUI.h"
#include "GameplayTag/KRItemTypeTag.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "GameplayTag/KRAbilityTag.h"

bool UKRUIAdapterLibrary::MakeUIDataFromItemInstance(const UKRInventoryItemInstance* Instance, FKRItemUIData& Out)
{
    Out = {};
    if (!Instance) return false;

    const UKRInventoryItemFragment* RawFrag = Instance->FindFragmentByTag(KRTAG_ABILITY_ITEM_DISPLAYUI);
    const UInventoryFragment_DisplayUI* UI = Cast<UInventoryFragment_DisplayUI>(RawFrag);
    if (!UI) return false;

    Out.ItemNameKey = UI->DisplayNameKey;
    Out.ItemDescriptionKey = UI->DescriptionKey;
    Out.ItemIcon = UI->ItemIcon;
    Out.Price = UI->Price;
    Out.UpgradeLevel = UI->WeaponLevel;
	Out.ItemTag = Instance->GetItemTag();

    if (UGameInstance* GI = UGameplayStatics::GetGameInstance(Instance))
    {
        if (UKRInventorySubsystem* Inv = GI->GetSubsystem<UKRInventorySubsystem>())
        {
            FGameplayTag ItemTag = Instance->GetItemTag();
            Out.Quantity = Inv->GetItemCountByTag(ItemTag);
        }
    }
    
    return true;
}

void UKRUIAdapterLibrary::ConvertItemInstancesToUIData(const TArray<UKRInventoryItemInstance*>& In, TArray<FKRItemUIData>& Out)
{
    Out.Reset(In.Num());
    for (const UKRInventoryItemInstance* I : In)
    {
        FKRItemUIData V;
        if (MakeUIDataFromItemInstance(I, V))
        {
            Out.Add(MoveTemp(V));
        }
    }
}

void UKRUIAdapterLibrary::GetOwnerInventoryUIData(UObject* WorldContextObject, TArray<FKRItemUIData>& Out)
{
    Out.Reset();
    if (!WorldContextObject) return;

    if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
    {
        if (auto* Inv = GI->GetSubsystem<UKRInventorySubsystem>())
        {
            const TArray<UKRInventoryItemInstance*> Items = Inv->GetAllItems();
            ConvertItemInstancesToUIData(Items, Out);
        }
    }
}