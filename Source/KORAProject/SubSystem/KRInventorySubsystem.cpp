#include "Subsystem/KRInventorySubsystem.h"

#include "MaterialPropertyHelpers.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "GameFramework/PlayerState.h"
#include "Engine/DataTable.h"
#include "GameFramework/AsyncAction_ListenForGameplayMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventorySubsystem)

FKRInventoryEntry* FKRInventoryList::CreateItem(FGameplayTag InTag)
{
	if (InTag.IsValid())
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
	if (InTag.IsValid())
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

const FKRInventoryEntry* FKRInventoryList::AddItem(FGameplayTag InTag, int32 StackCount)
{
	if (InTag.IsValid())
	{
		return nullptr;
	}
	
	FKRInventoryEntry* Entry = Entries.Find(InTag);
	if (Entry)
	{
		Entry->AddCount(StackCount);
	}
	else
	{
		Entry = CreateItem(InTag);
	}

	return Entry;
}

void FKRInventoryList::SubtractItem(FGameplayTag InTag, int32 StackCount)
{
	if (InTag.IsValid())
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

const FKRInventoryEntry* FKRInventoryList::GetItem(FGameplayTag InTag) const
{
	if (InTag.IsValid())
	{
		return nullptr;
	}

	const FKRInventoryEntry* Entry = Entries.Find(InTag);

	return Entry;
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
}

void UKRInventorySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TSubclassOf<UKRInventoryItemFragment> UKRInventorySubsystem::GetFragmentClass(FGameplayTag Tag) const
{
	if (Tag.IsValid())
	{
		return nullptr;
	}
	
	if (const TSubclassOf<UKRInventoryItemFragment>* Found = FragmentRegistry.Find(Tag))
	{
		return *Found;
	}
	
	return nullptr;
}

int32 UKRInventorySubsystem::LoadItemDefinitionsFromDataTable()
{
	// if (!DataTable)
	// {
	// 	return 0;
	// }
	//
	// int32 LoadedCount = 0;
	// TArray<FKRItemTableRow*> Rows;
	// DataTable->GetAllRows<FKRItemTableRow>(TEXT("LoadItemDefinitions"), Rows);
	//
	// TArray<FName> RowNames = DataTable->GetRowNames();
	//
	// for (int32 i = 0; i < Rows.Num(); ++i)
	// {
	// 	const FKRItemTableRow* Row = Rows[i];
	// 	if (Row && Row->ItemDefinition)
	// 	{
	// 		// DataTable의 Row Name을 GameplayTag로 변환
	// 		const FName RowName = RowNames[i];
	// 		const FGameplayTag ItemTag = FGameplayTag::RequestGameplayTag(RowName, false);
	//
	// 		if (ItemTag.IsValid())
	// 		{
	// 			// Definition CDO 가져오기
	// 			UKRInventoryItemDefinition* DefCDO = Row->ItemDefinition.GetDefaultObject();
	//
	// 			// CDO의 FragmentContainer가 비어있으면 초기화 (한 번만!)
	// 			if (DefCDO && DefCDO->FragmentContainer.IsEmpty())
	// 			{
	// 				InitializeItemDefinitionFragments(DefCDO, Row->AbilityTags);
	// 			}
	//
	// 			// 레지스트리에 등록
	// 			RegisterItemDefinition(ItemTag, Row->ItemDefinition, Row->AbilityTags);
	// 			LoadedCount++;
	// 		}
	// 	}
	// }

	return 0;
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
	for (const TPair<TObjectPtr<APlayerState>, FKRInventoryList>& Pair : PlayerInventories)
	{
		const FKRInventoryEntry* Entry = Pair.Value.GetItem(ItemTag);
		if (Entry)
		{
			const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
			if (!ItemInstance)
			{
				break;
			}

			ItemDefinition = ItemInstance->GetItemDef();
			
			break;
		}
	}
	
	return ItemDefinition;
}

const UKRInventoryItemFragment* UKRInventorySubsystem::GetItemFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag) const
{
	const UKRInventoryItemFragment* Fragment = nullptr;
	for (const TPair<TObjectPtr<APlayerState>, FKRInventoryList>& Pair : PlayerInventories)
	{
		const FKRInventoryEntry* Entry = Pair.Value.GetItem(ItemTag);
		if (Entry)
		{
			const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
			if (!ItemInstance)
			{
				break;
			}

			Fragment = ItemInstance->FindFragmentByTag(FragmentTag);
			break;
		}
	}
	
	return Fragment;
}

FKRInventoryList& UKRInventorySubsystem::GetOrCreateInventory(APlayerState* Owner)
{
	if (!PlayerInventories.Contains(Owner))
	{
		PlayerInventories.Add(Owner, FKRInventoryList());
	}
	
	return PlayerInventories[Owner];
}

void UKRInventorySubsystem::InitializeItemDefinitionFragments(const FGameplayTagContainer& AbilityTags)
{
	FragmentRegistry.Empty();

	// Fragment 마다 StaticClass를 매개변수로 넣어주기만 하면 끝 
	//InitialFragmentType();
}

UKRInventoryItemInstance* UKRInventorySubsystem::AddItem(APlayerState* Owner, FGameplayTag ItemTag, int32 StackCount)
{
	if (!Owner)
	{
		return nullptr;
	}

	const FKRInventoryEntry* Entry = GetOrCreateInventory(Owner).AddItem(ItemTag, StackCount);

	// Gameplay Message Subsystem Broadcast Sample
	FAddItemMessage AddItemMessage;
	AddItemMessage.ItemTag = FGameplayTag::RequestGameplayTag(FName("ItemType.Weapon"));
	AddItemMessage.StackCount = 10;

	FGameplayTag AddItemTag = FGameplayTag::RequestGameplayTag(FName("Interact.AddItem"));
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage<FAddItemMessage>(AddItemTag, AddItemMessage);

	// Gameplay Message Subsystem Listen Sample
	UAsyncAction_ListenForGameplayMessage::ListenForGameplayMessages(this, AddItemTag, FAddItemMessage::StaticStruct());
	
	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

void UKRInventorySubsystem::SubtractItem(APlayerState* Owner, FGameplayTag ItemTag, int32 StackCount)
{
	if (!Owner)
	{
		return;
	}

	GetOrCreateInventory(Owner).SubtractItem(ItemTag, StackCount);
}

void UKRInventorySubsystem::RemoveItem(APlayerState* Owner, FGameplayTag InTag)
{
	if (!Owner)
	{
		return;
	}

	GetOrCreateInventory(Owner).RemoveItem(InTag);
}

const UKRInventoryItemInstance* UKRInventorySubsystem::GetItem(APlayerState* Owner, FGameplayTag InTag)
{
	if (!Owner)
	{
		return nullptr;
	}

	const FKRInventoryEntry* Entry = GetOrCreateInventory(Owner).GetItem(InTag);

	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

int32 UKRInventorySubsystem::GetItemCountByTag(APlayerState* Owner, FGameplayTag InTag)
{
	int32 StackCount = 0;
	if (Owner)
	{
		if (const FKRInventoryEntry* Entry = GetOrCreateInventory(Owner).GetItem(InTag))
		{
			StackCount = Entry->GetStackCount();
		}
	}

	return StackCount;
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::GetAllItems(APlayerState* Owner) const
{
	if (!Owner)
	{
		return TArray<UKRInventoryItemInstance*>();
	}

	const FKRInventoryList* Inventory = PlayerInventories.Find(Owner);
	if (!Inventory)
	{
		return TArray<UKRInventoryItemInstance*>();
	}

	return Inventory->GetAllItems();
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::FindItemsByTag(APlayerState* Owner, FGameplayTag ItemTag) const
{
	TArray<UKRInventoryItemInstance*> Items;

	if (!Owner || !ItemTag.IsValid())
	{
		return Items;
	}

	const FKRInventoryList* Inventory = PlayerInventories.Find(Owner);
	if (!Inventory)
	{
		return Items;
	}

	return Inventory->FindAllItemsByTag(ItemTag);
}

void UKRInventorySubsystem::ClearInventory(APlayerState* Owner)
{
	if (!Owner)
	{
		return;
	}

	FKRInventoryList* Inventory = PlayerInventories.Find(Owner);
	if (Inventory)
	{
		Inventory->Clear();
	}
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

void UKRInventorySubsystem::RegisterFragmentClass(FGameplayTag Tag, TSubclassOf<UKRInventoryItemFragment> FragmentClass)
{
	if (Tag.IsValid() && FragmentClass)
	{
		FragmentRegistry.Add(Tag, FragmentClass);
	}
}
