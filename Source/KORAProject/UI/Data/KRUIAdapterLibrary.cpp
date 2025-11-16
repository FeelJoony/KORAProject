// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Data/KRUIAdapterLibrary.h"
//#include "Inventory/KRInventoryItemInstance.h"
//#include "SubSystem/KRInventorySubsystem.h"

// fragments classes
#include "Kismet/GameplayStatics.h"

//bool UKRUIAdapterLibrary::MakeUIDataFromItemInstance(const UKRInventoryItemInstance* Instance, FKRItemUIData& Out)
//{
//    Out = {};
//    if (!Instance) return false;
//
//    Out.ItemID = Instance->GetItemTag().GetTagName();
//    Out.Quantity = Instance->GetStackCount();
//
//    if (const UKRInvFragment* UI = Instance->FindFragment<UKRInvFragment>())
//    {
//        Out.ItemNameKey = UI->ItemName;
//        Out.ItemDescriptionKey = UI->Description;
//        Out.ItemIcon = UI->Icon;
//    }
//
//
//    return true;
//}
//
//void UKRUIAdapterLibrary::ConvertItemInstancesToUIData(const TArray<UKRInventoryItemInstance*>& In, TArray<FKRItemUIData>& Out)
//{
//    Out.Reset(In.Num());
//    for (const UKRInventoryItemInstance* I : In)
//    {
//        FKRItemUIData V;
//        if (MakeUIDataFromItemInstance(I, V))
//        {
//            Out.Add(MoveTemp(V));
//        }
//    }
//}
//
//void UKRUIAdapterLibrary::GetOwnerInventoryUIData(UObject* WorldContextObject, APlayerState* Owner, TArray<FKRItemUIData>& Out)
//{
//    Out.Reset();
//    if (!WorldContextObject || !Owner) return;
//
//    if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
//    {
//        if (auto* Inv = GI->GetSubsystem<UKRInventorySubsystem>())
//        {
//            const TArray<UKRInventoryItemInstance*> Items = Inv->GetAllItems(Owner);
//            ConvertItemInstancesToUIData(Items, Out);
//        }
//    }
//}