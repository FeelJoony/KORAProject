// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Data/KRUIAdapterLibrary.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "SubSystem/KRInventorySubsystem.h"
//#include "Inventrory/InventoryFragment_DisplayUI.h"
#include "GAS/KRFragmentTag.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventoryItemInstance)

bool UKRUIAdapterLibrary::MakeUIDataFromItemInstance(const UKRInventoryItemInstance* Instance, FKRItemUIData& Out)
{
    Out = {};
    if (!Instance) return false;

    const UKRInventoryItemFragment* RawFrag = Instance->FindFragmentByTag(KRFragmentTag::Tag_ItemFragment_UI);
    const InventoryFragment_DisplayUI* UI = Cast<InventoryFragment_DisplayUI>(RawFrag);
    if (!UI) return false;

    Out.ItemNameKey = UI->DisplayNameKey;
    Out.ItemDescriptionKey = UI->DescriptionKey;
    Out.ItemIcon = UI->ItemIcon;
    Out.Price = UI->Price;
    Out.UpgradeLevel = UI->WeaponLevel;
    
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