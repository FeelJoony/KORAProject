// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Data/KRItemUIData.h"

//#include "KRBaseItem.h"


//FKRItemUIData UKRUIAdapterLibrary::MakeUIDataFromItem(UKRBaseItem* Item)
//{
//    FKRItemUIData D;
//    if (!Item) return D;
//
//    D.SourceItem = Item;
//    D.ItemID = Item->GetItemID();
//    D.ItemName = Item->GetItemName();
//    D.ItemDescription = Item->GetItemDescription();
//    D.ItemIcon = Item->GetItemIcon();
//    D.Quantity = Item->GetStackCount();
//    D.Price = Item->GetBasePrice();
//	  if (Item.GetTypeTag() == FGameplayTag::RequestGameplayTag(TEXT("ItemType.Equip.Gun")) || Item.GetTypeTag() == FGameplayTag::RequestGameplayTag(TEXT("ItemType.Equip.Sword"))) 
//    { D.UpgradeLevel = Item->GetEnforceLevel(); }
//
//    return D;
//}
//
//void UKRUIAdapterLibrary::ConvertItemsToUIData(const TArray<UKRBaseItem*>& In, TArray<FKRItemUIData>& Out)
//{
//	Out.Empty();
//	for (UKRBaseItem* Item : In)
//	{
//		Out.Add(MakeUIDataFromItem(Item));
//	}
//}
