#include "Subsystem/KRInventorySubsystem.h"

#include "MaterialPropertyHelpers.h"
#include "NativeGameplayTags.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "GameFramework/PlayerState.h"
#include "Data/CacheDataTable.h"
#include "Data/ItemDataStruct.h"
#include "GameFramework/AsyncAction_ListenForGameplayMessage.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "Inventory/InventoryFragment_SetStats.h"
#include "Inventory/InventoryFragment_SetIcon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventorySubsystem)

DEFINE_LOG_CATEGORY(LogInventorySubSystem);

FKRInventoryEntry* FKRInventoryList::CreateItem(FGameplayTag InTag)
{
	if (!InTag.IsValid())
	{
		return nullptr;
	}
	
	if (Entries.Contains(InTag))
	{
		return nullptr;
	}

	ItemTagContainer.AddTag(InTag);
	
	Entries.Add(InTag, FKRInventoryEntry(UKRInventoryItemInstance::CreateItemInstance()->CreateItemDefinition()));

	return &Entries[InTag];
}

void FKRInventoryList::RemoveItem(FGameplayTag InTag)
{
	if (!InTag.IsValid())
	{
		return;
	}
	
	if (false == Entries.Contains(InTag))
	{
		return;
	}
	
	ItemTagContainer.RemoveTag(InTag);
	
	Entries.Remove(InTag);
}

const FKRInventoryEntry* FKRInventoryList::AddItem(FGameplayTag InTag, int32 StackCount, bool& OutIsNew)
{
	if (!InTag.IsValid())
	{
		return nullptr;
	}
	
	FKRInventoryEntry* Entry = Entries.Find(InTag);
	if (Entry)
	{
		Entry->AddCount(StackCount);

		OutIsNew = false;
	}
	else
	{
		Entry = CreateItem(InTag);

		OutIsNew = true;
	}

	return Entry;
}

void FKRInventoryList::SubtractItem(FGameplayTag InTag, int32 StackCount)
{
	if (!InTag.IsValid())
	{
		return;
	}
	
	FKRInventoryEntry* Entry = Entries.Find(InTag);
	if (!Entry)
	{
		return;
	}

	Entry->SubtractCount(StackCount);
	if (Entry->GetStackCount() <= 0)
	{
		RemoveItem(InTag);
	}
}

FKRInventoryEntry* FKRInventoryList::GetItem(FGameplayTag InTag)
{
	check(InTag.IsValid());

	return &Entries[InTag];
}

const FKRInventoryEntry* FKRInventoryList::GetItem(FGameplayTag InTag) const
{
	check(InTag.IsValid());

	return &Entries[InTag];
}

TArray<UKRInventoryItemInstance*> FKRInventoryList::GetAllItems() const
{
	TArray<UKRInventoryItemInstance*> Instances;
	for (const TPair<FGameplayTag, FKRInventoryEntry>& Pair : Entries)
	{
		if (UKRInventoryItemInstance* Instance = Pair.Value.GetItemInstance())
		{
			Instances.Add(Instance);
		}
	}

	return Instances;
}

TArray<UKRInventoryItemInstance*> FKRInventoryList::FindAllItemsByTag(FGameplayTag FilterTag) const
{
	TArray<UKRInventoryItemInstance*> FindInstances;
	if (!ItemTagContainer.IsValid())
	{
		return FindInstances;
	}

	FGameplayTagContainer FilterTagContainer(FilterTag);
	const FGameplayTagContainer& ResultTagContainer = ItemTagContainer.Filter(FilterTagContainer);
	for (const FGameplayTag& ItemTag : ResultTagContainer.GetGameplayTagArray())
	{
		if (UKRInventoryItemInstance* FindInstance = Entries[ItemTag].GetItemInstance())
		{
			FindInstances.Add(FindInstance);
		}
	}

	return FindInstances;
}

void FKRInventoryList::Clear()
{
	Entries.Empty();
}

void UKRInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UKRDataTablesSubsystem>();

	InitializeItemDefinitionFragments();
}

void UKRInventorySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TSubclassOf<UKRInventoryItemFragment> UKRInventorySubsystem::GetFragmentClass(FGameplayTag Tag) const
{
	if (!Tag.IsValid())
	{
		return nullptr;
	}
	
	if (const TSubclassOf<UKRInventoryItemFragment>* Found = FragmentRegistry.Find(Tag))
	{
		return *Found;
	}
	
	return nullptr;
}

void UKRInventorySubsystem::InitFragment(FGameplayTag ItemTag)
{
	UKRDataTablesSubsystem* DataTableSubsystem = GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataTableSubsystem)
	{
		return;
	}

	UCacheDataTable* EquipCacheDataTable = DataTableSubsystem->GetTable(EGameDataType::EquipData);

	FItemDataStruct* ItemData = DataTableSubsystem->GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag);

	for (const FGameplayTag& AbilityTag : ItemData->AbilityTags)
	{
		if (!FragmentRegistry.Contains(AbilityTag))
		{
			UE_LOG(LogInventorySubSystem, Warning, TEXT("No exist %s Tag. You must be create this tag Fragment"), *AbilityTag.GetTagName().ToString());
				
			continue;
		}
		
		AddFragment(ItemData->TypeTag, AbilityTag);
	}
}

void UKRInventorySubsystem::AddFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag)
{
	TSubclassOf<UKRInventoryItemFragment> FragmentClass = FragmentRegistry[FragmentTag]; 
	UKRInventoryItemFragment* Fragment = FragmentClass->GetDefaultObject<UKRInventoryItemFragment>();
	
	FKRInventoryEntry* Entry = GetInventory().GetItem(ItemTag);
	Entry->GetItemInstance()->GetItemDef()->AddFragment(FragmentTag, Fragment);
}

TArray<FGameplayTag> UKRInventorySubsystem::GetAllRegisteredItemTags() const
{
	TArray<FGameplayTag> OutTags;
	FragmentRegistry.GetKeys(OutTags);
	
	return OutTags;
}

const UKRInventoryItemDefinition* UKRInventorySubsystem::GetItemDefinition(FGameplayTag ItemTag) const
{
	const UKRInventoryItemDefinition* ItemDefinition = nullptr;
	const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
	const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
	if (!ItemInstance)
	{
		return nullptr;
	}

	ItemDefinition = ItemInstance->GetItemDef();
			
	return ItemDefinition;
}

const UKRInventoryItemFragment* UKRInventorySubsystem::GetItemFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag) const
{
	const UKRInventoryItemFragment* Fragment = nullptr;
	const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
	if (Entry)
	{
		const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
		if (!ItemInstance)
		{
			return nullptr;
		}

		Fragment = ItemInstance->FindFragmentByTag(FragmentTag);
	}
	
	return Fragment;
}

FKRInventoryList& UKRInventorySubsystem::GetInventory()
{
	return PlayerInventory;
}

void UKRInventorySubsystem::InitializeItemDefinitionFragments()
{
	FragmentRegistry.Empty();

	// Fragment 마다 StaticClass를 매개변수로 넣어주기만 하면 끝 
	InitialFragmentType(UInventoryFragment_EquippableItem::StaticClass());
	InitialFragmentType(UInventoryFragment_SetStats::StaticClass());
	InitialFragmentType(UInventoryFragment_SetIcon::StaticClass());
}

UKRInventoryItemInstance* UKRInventorySubsystem::AddItem(FGameplayTag ItemTag, int32 StackCount)
{
	bool bIsNew = false;
	const FKRInventoryEntry* Entry = GetInventory().AddItem(ItemTag, StackCount, bIsNew);
	if (bIsNew)
	{
		InitFragment(ItemTag);
	}

	// // Gameplay Message Subsystem Broadcast Sample
	// FAddItemMessage AddItemMessage;
	// AddItemMessage.ItemTag = FGameplayTag::RequestGameplayTag(FName("ItemType.Weapon"));
	// AddItemMessage.StackCount = 10;
	//
	// FGameplayTag AddItemTag = FGameplayTag::RequestGameplayTag(FName("Interact.AddItem"));
	//
	// UGameplayMessageSubsystem::Get(this).BroadcastMessage<FAddItemMessage>(AddItemTag, AddItemMessage);
	//
	// // Gameplay Message Subsystem Listen Sample
	// UAsyncAction_ListenForGameplayMessage::ListenForGameplayMessages(this, AddItemTag, FAddItemMessage::StaticStruct());
	
	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

void UKRInventorySubsystem::SubtractItem(FGameplayTag ItemTag, int32 StackCount)
{
	GetInventory().SubtractItem(ItemTag, StackCount);
}

void UKRInventorySubsystem::RemoveItem(FGameplayTag InTag)
{
	GetInventory().RemoveItem(InTag);
}

const UKRInventoryItemInstance* UKRInventorySubsystem::GetItem(FGameplayTag InTag)
{
	const FKRInventoryEntry* Entry = GetInventory().GetItem(InTag);

	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

int32 UKRInventorySubsystem::GetItemCountByTag(FGameplayTag InTag)
{
	int32 StackCount = 0;
	if (const FKRInventoryEntry* Entry = GetInventory().GetItem(InTag))
	{
		StackCount = Entry->GetStackCount();
	}

	return StackCount;
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::GetAllItems() const
{
	return PlayerInventory.GetAllItems();
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::FindItemsByTag(FGameplayTag ItemTag) const
{
	TArray<UKRInventoryItemInstance*> Items;

	if (!ItemTag.IsValid())
	{
		return Items;
	}

	return PlayerInventory.FindAllItemsByTag(ItemTag);
}

void UKRInventorySubsystem::ClearInventory()
{
	PlayerInventory.Clear();
}

void UKRInventorySubsystem::InitialFragmentType(TSubclassOf<UKRInventoryItemFragment> FragmentClass)
{
	const UKRInventoryItemFragment* DefaultFragment = FragmentClass->GetDefaultObject<UKRInventoryItemFragment>();

	if (DefaultFragment)
	{
		FGameplayTag FragmentTag = DefaultFragment->GetFragmentTag();
		FragmentRegistry.Add(FragmentTag, FragmentClass);
	}
}
