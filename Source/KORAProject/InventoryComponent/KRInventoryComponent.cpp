#include "InventoryComponent/KRInventoryComponent.h"
#include "Item/KRBaseItem.h"
#include "Item/KREquipItem.h"
#include "Item/KRWeaponItem.h"
#include "Item/KRModuleItem.h"
#include "Item/KRConsumeItem.h"

UKRInventoryComponent::UKRInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKRInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	RebuildIndex();
}

void UKRInventoryComponent::IndexItem(UKRBaseItem* InNewItem)
{
	if (!IsValid(InNewItem)) return;

	const FGameplayTagContainer& ItemTags = InNewItem->GetItemTags(); 
	
	// for (const FGameplayTag& Tag : ItemTags)
	//
	// 	if (Tag.MatchesTag(TAG_ItemType)) 
	// 	{
	// 		ItemsByCategory.FindOrAdd(Tag).Items.AddUnique(InNewItem);
	// 	}
	// }
	//
	// if (UKRWeaponItem* Weapon = Cast<UKRWeaponItem>(InNewItem))
	// {
	// 	for (const FGameplayTag& Tag : ItemTags)
	// 	{
	// 		if (Tag.MatchesTag(TAG_ItemType_Equip_Sword) || Tag.MatchesTag(TAG_ItemType_Equip_Gun)) 
	// 		{
	// 			ItemsByWeaponType.FindOrAdd(Tag).Items.AddUnique(Weapon);
	// 		}
	// 	}
	// }
	//
	// if (UKRModuleItem* Module = Cast<UKRModuleItem>(InNewItem))
	// {
	// 	for (const FGameplayTag& Tag : ItemTags)
	// 	{
	// 		if (Tag.MatchesTag(TAG_ItemType_Equip_GunModule) || Tag.MatchesTag(TAG_ItemType_Equip_SwordModule))
	// 		{
	// 			ItemsByModuleType.FindOrAdd(Tag).Items.AddUnique(Module);
	// 		}
	// 	}
	// }
}

void UKRInventoryComponent::UnindexItem(UKRBaseItem* InItem)
{
	if (!IsValid(InItem)) return;

	const FGameplayTagContainer& ItemTags = InItem->GetItemTags();
	
	// for (const FGameplayTag& Tag : ItemTags)
	// {
	// 	if (Tag.MatchesTag(TAG_ItemType)) 
	// 	{
	// 		if (FKRBaseItemArrayWrapper* FoundWrapper = ItemsByCategory.Find(Tag))
	// 		{
	// 			FoundWrapper->Items.Remove(InItem);
	// 		}
	// 	}
	// }
	//
	// if (UKRWeaponItem* Weapon = Cast<UKRWeaponItem>(InItem))
	// {
	// 	for (const FGameplayTag& Tag : ItemTags)
	// 	{
	// 		if (Tag.MatchesTag(TAG_ItemType_Equip_Sword) || Tag.MatchesTag(TAG_ItemType_Equip_Gun))
	// 		{
	// 			if (FKRWeaponItemArrayWrapper* FoundWrapper = ItemsByWeaponType.Find(Tag))
	// 			{
	// 				FoundWrapper->Items.Remove(Weapon);
	// 			}
	// 		}
	// 	}
	// }
	//
	// if (UKRModuleItem* Module = Cast<UKRModuleItem>(InItem))
	// {
	// 	for (const FGameplayTag& Tag : ItemTags)
	// 	{
	// 		if (Tag.MatchesTag(TAG_ItemType_Equip_GunModule) || Tag.MatchesTag(TAG_ItemType_Equip_SwordModule)) 
	// 		{
	// 			if (FKRModuleItemArrayWrapper* FoundWrapper = ItemsByModuleType.Find(Tag))
	// 			{
	// 				FoundWrapper->Items.Remove(Module);
	// 			}
	// 		}
	// 	}
	// }
}

void UKRInventoryComponent::RebuildIndex()
{
	ItemsByCategory.Empty();
	ItemsByWeaponType.Empty();
	ItemsByModuleType.Empty();

	for (UKRBaseItem* Item : AllItems)
	{
		IndexItem(Item);
	}
}

UKRBaseItem* UKRInventoryComponent::FindExistingStack(UKRBaseItem* InItemToFind) const
{
	if (!IsValid(InItemToFind) || InItemToFind->GetMaxStackSize() <= 1)
	{
		return nullptr;
	}
	
	for (UKRBaseItem* Item : AllItems)
	{
		if (IsValid(Item) &&
			Item->GetItemID() == InItemToFind->GetItemID() && 
			Item->GetAmount() < Item->GetMaxStackSize())
		{
			return Item;
		}
	}
	return nullptr;
}

bool UKRInventoryComponent::AddItem(UKRBaseItem* InNewItem)
{
	if (!IsValid(InNewItem)) return false;

	UKRBaseItem* ExistingStack = FindExistingStack(InNewItem);
	if (ExistingStack)
	{
		int32 AmountToAdd = FMath::Min(InNewItem->GetAmount(), ExistingStack->GetMaxStackSize() - ExistingStack->GetAmount());
		ExistingStack->SetAmount(ExistingStack->GetAmount() + AmountToAdd);
		
		int32 RemainingAmount = InNewItem->GetAmount() - AmountToAdd;
		if (RemainingAmount > 0)
		{
			InNewItem->SetAmount(RemainingAmount);
			return AddItem(InNewItem);
		}
		else
		{
			InNewItem->MarkAsGarbage();

			return true;
		}
	}
	else
	{
		AllItems.Add(InNewItem);
		IndexItem(InNewItem);

		return true;
	}
}

bool UKRInventoryComponent::RemoveItem(UKRBaseItem* InItem, int32 InCount)
{
	if (!IsValid(InItem) || !AllItems.Contains(InItem)) return false;

	const int32 NewAmount = InItem->GetAmount() - InCount;
	if (NewAmount > 0)
	{
		InItem->SetAmount(NewAmount);
	}
	else
	{
		UnindexItem(InItem);
		AllItems.Remove(InItem);
		InItem->MarkAsGarbage();
	}

	return true;
}

TArray<UKRBaseItem*> UKRInventoryComponent::GetItemsByTag(FGameplayTag InFilterTag) const
{
	if (const FKRBaseItemArrayWrapper* FoundWrapper = ItemsByCategory.Find(InFilterTag))
	{
		TArray<UKRBaseItem*> Result;
		Result.Reserve(FoundWrapper->Items.Num()); 
		for(const TObjectPtr<UKRBaseItem>& ItemPtr : FoundWrapper->Items)
		{
			Result.Add(ItemPtr.Get());
		}
		return Result;
	}
	return TArray<UKRBaseItem*>();
}

TArray<UKRBaseItem*> UKRInventoryComponent::GetItemsByTagsAll(const TArray<FGameplayTag>& InFilterTags) const
{
	TArray<UKRBaseItem*> Result;
	const FGameplayTagContainer TagContainer = FGameplayTagContainer::CreateFromArray(InFilterTags);
	
	for (UKRBaseItem* Item : AllItems)
	{
		if (IsValid(Item) && Item->GetItemTags().HasAll(TagContainer))
		{
			Result.Add(Item);
		}
	}
	return Result;
}

TArray<UKRBaseItem*> UKRInventoryComponent::GetItemsByTagsAny(const TArray<FGameplayTag>& InFilterTags) const
{
	TArray<UKRBaseItem*> Result;
	const FGameplayTagContainer TagContainer = FGameplayTagContainer::CreateFromArray(InFilterTags);

	for (UKRBaseItem* Item : AllItems)
	{
		if (IsValid(Item) && Item->GetItemTags().HasAny(TagContainer))
		{
			Result.Add(Item);
		}
	}
	return Result;
}

TArray<UKRModuleItem*> UKRInventoryComponent::GetCompatibleModules(UKRWeaponItem* InWeapon) const
{
	TArray<UKRModuleItem*> Result;
	if (!IsValid(InWeapon)) return Result;
	
	return Result;
}

bool UKRInventoryComponent::EquipItem(UKREquipItem* InItem, FGameplayTag InSlotTag)
{
	if (!IsValid(InItem) || !AllItems.Contains(InItem)) return false; 

	UnequipItem(InSlotTag);

	EquippedItems.Add(InSlotTag, InItem);
	InItem->Equip(GetOwner()); 

	return true;
}

bool UKRInventoryComponent::UnequipItem(FGameplayTag InSlotTag)
{
	if (!EquippedItems.Contains(InSlotTag)) return false;

	UKREquipItem* ItemToUnequip = EquippedItems[InSlotTag];
	if (IsValid(ItemToUnequip))
	{
		ItemToUnequip->Unequip(GetOwner());
	}
	EquippedItems.Remove(InSlotTag);
	
	return true;
}

void UKRInventoryComponent::RegisterQuickSlot(int32 InSlotIndex, UKRConsumeItem* InItem)
{
	if (IsValid(InItem))
	{
		if (!AllItems.Contains(InItem)) return;
		QuickSlots.Add(InSlotIndex, InItem);
	}
	else
	{
		QuickSlots.Remove(InSlotIndex);
	}
}

bool UKRInventoryComponent::UseQuickSlot(int32 InSlotIndex)
{
	if (QuickSlots.Contains(InSlotIndex))
	{
		UKRConsumeItem* ItemToUse = QuickSlots[InSlotIndex];
		if (IsValid(ItemToUse))
		{
			ItemToUse->Use(GetOwner()); 
			
			const bool bItemRemoved = RemoveItem(ItemToUse, 1);

			if (bItemRemoved && !IsValid(ItemToUse))
			{
				QuickSlots.Remove(InSlotIndex);
			}
			return true;
		}
	}
	return false;
}

