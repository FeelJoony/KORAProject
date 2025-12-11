// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Data/KRUIAdapterLibrary.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "SubSystem/KRInventorySubsystem.h"
#include "SubSystem/KRShopSubsystem.h"
#include "Inventory/Fragment/InventoryFragment_DisplayUI.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Equipment/KREquipmentManagerComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "GameplayTag/KRAbilityTag.h"

bool UKRUIAdapterLibrary::MakeUIDataFromItemInstance(const UKRInventoryItemInstance* Instance, FKRItemUIData& Out)
{
    Out = {};
    if (!Instance) return false;

    const UKRInventoryItemFragment* RawFrag = Instance->FindFragmentByTag(KRTAG_ABILITY_ITEM_DISPLAYUI);
    const UInventoryFragment_DisplayUI* UI = Cast<UInventoryFragment_DisplayUI>(RawFrag);
    if (!UI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UIAdapter] No DisplayUI fragment found for item: %s"), *Instance->GetItemTag().ToString());
        return false;
    }

    Out.ItemNameKey = UI->DisplayNameKey;
    Out.ItemDescriptionKey = UI->DescriptionKey;
    Out.ItemIcon = UI->ItemIcon;
    Out.Price = UI->Price;
    Out.UpgradeLevel = UI->WeaponLevel;
	Out.ItemTag = Instance->GetItemTag();

    UObject* OwnerContext = Instance->GetOwnerContext();
    if (UGameInstance* GI = OwnerContext ? Cast<UGameInstance>(OwnerContext) : nullptr)
    {
        if (UKRInventorySubsystem* Inv = GI->GetSubsystem<UKRInventorySubsystem>())
        {
            Out.Quantity = Inv->GetItemCountByTag(Instance->GetItemTag());
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

void UKRUIAdapterLibrary::GetInventoryUIDataFiltered(UObject* WorldContextObject, const FGameplayTag& FilterTag, TArray<FKRItemUIData>& Out)
{
    Out.Reset();
    if (!WorldContextObject) return;

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (auto* Inv = GI->GetSubsystem<UKRInventorySubsystem>())
		{
			TArray<UKRInventoryItemInstance*> Items;
			if (!FilterTag.IsValid())
			{
				Items = Inv->GetAllItems();
			}
			else
			{
				Items = Inv->FindItemsByTag(FilterTag);
			}
			ConvertItemInstancesToUIData(Items, Out);
		}
	}
}

void UKRUIAdapterLibrary::GetShopUIData(UObject* WorldContextObject, TArray<FKRItemUIData>& Out)
{
    Out.Reset();
    if (!WorldContextObject) return;

    if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
    {
        if (UKRShopSubsystem* Shop = GI->GetSubsystem<UKRShopSubsystem>())
        {
            const TArray<UKRInventoryItemInstance*>& Items = Shop->GetShopStock();
            ConvertItemInstancesToUIData(Items, Out);

            for (FKRItemUIData& Data : Out)
            {
                if (!Data.ItemTag.IsValid())
                {
                    Data.ShopStock = 0;
                    continue;
                }
                Data.ShopStock = Shop->GetStockCountByItemTag(Data.ItemTag);
            }
        }
    }
}

void UKRUIAdapterLibrary::GetEquippedCategoryUIData(UObject* WorldContextObject, const TArray<FGameplayTag>& SlotTagOrder, TArray<FKRItemUIData>& Out)
{
    Out.Reset();
    if (!WorldContextObject) return;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;
    UKREquipmentManagerComponent* EquipMgr = Pawn ? Pawn->FindComponentByClass<UKREquipmentManagerComponent>() : nullptr;

    Out.SetNum(SlotTagOrder.Num());

    for (int32 i = 0; i < SlotTagOrder.Num(); ++i)
    {
        const FGameplayTag SlotTag = SlotTagOrder[i];

        FKRItemUIData UIData;
        if (EquipMgr)
        {
            if (UKRInventoryItemInstance* Inst = EquipMgr->GetEquippedItemInstanceBySlotTag(SlotTag))
            {
                MakeUIDataFromItemInstance(Inst, UIData);
            }
        }

        Out[i] = UIData;
    }
}

bool UKRUIAdapterLibrary::GetEquippedSlotUIData(UObject* WorldContextObject, const FGameplayTag& SlotTag, FKRItemUIData& Out)
{
    if (!WorldContextObject) return false;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return false;

    APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;
    UKREquipmentManagerComponent* EquipMgr = Pawn ? Pawn->FindComponentByClass<UKREquipmentManagerComponent>() : nullptr;

    if (!EquipMgr) return false;
    
    if (UKRInventoryItemInstance* Inst = EquipMgr->GetEquippedItemInstanceBySlotTag(SlotTag))
    {
        FKRItemUIData UIData;
        if (MakeUIDataFromItemInstance(Inst, UIData))
        {
            Out = UIData;
            return true;
        }
    }
    return false;
}

