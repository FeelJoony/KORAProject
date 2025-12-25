#include "Subsystem/KRInventorySubsystem.h"

#include "Player/KRPlayerState.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Data/CacheDataTable.h"
#include "Data/ItemDataStruct.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/AsyncAction_ListenForGameplayMessage.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GameplayTag/KRPlayerTag.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "Inventory/Fragment/InventoryFragment_DisplayUI.h"
#include "Inventory/Fragment/InventoryFragment_SellableItem.h"
#include "Inventory/Fragment/InventoryFragment_ConsumableItem.h"
#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "Inventory/Fragment/InventoryFragment_QuickSlot.h"
#include "Inventory/Fragment/InventoryFragment_ModuleItem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventorySubsystem)

DEFINE_LOG_CATEGORY(LogInventorySubSystem);

namespace
{
	const TArray<FGameplayTag>& GetQuickSlotOrder()
	{
		static TArray<FGameplayTag> SlotOrder = {
			FKRUIMessageTags::QuickSlot_North(),
			FKRUIMessageTags::QuickSlot_East(),
			FKRUIMessageTags::QuickSlot_South(),
			FKRUIMessageTags::QuickSlot_West()
		};
		return SlotOrder;
	}
	
	template <typename Func>
	void ForEachQuickSlot(TMap<FGameplayTag, FGameplayTag>& Bindings, Func&& InFunc)
	{
		for (auto& Pair : Bindings)
		{
			InFunc(Pair.Key, Pair.Value);
		}
	}
	
	template <typename Func>
	void ForEachQuickSlot(const TMap<FGameplayTag, FGameplayTag>& Bindings, Func&& InFunc)
	{
		for (const auto& Pair : Bindings)
		{
			InFunc(Pair.Key, Pair.Value);
		}
	}

	static const UInventoryFragment_ConsumableItem* GetConsumableFragmentFromItem(const FKRInventoryEntry* Entry)
	{
		if (!Entry) return nullptr;

		const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
		if (!ItemInstance) return nullptr;

		const UKRInventoryItemFragment* Frag = ItemInstance->FindFragmentByTag(KRTAG_FRAGMENT_ITEM_CONSUMABLE);

		return Cast<UInventoryFragment_ConsumableItem>(Frag);
	}

	float GetConsumableUITime(const UInventoryFragment_ConsumableItem* Frag)
	{
		if (!Frag) { return 0.f; }

		const float Cooldown  = Frag->CooldownConfig.ExtraCooldown;
		const float Duration  = Frag->EffectConfig.Duration;
		
		if (Cooldown > 0.f) { return Cooldown; }
		
		if (Duration > 0.f) { return Duration; }
		
		return 0.f;
	}
}

FKRInventoryEntry* FKRInventoryList::CreateItem(const FGameplayTag& InTag)
{
	if (!InTag.IsValid()) { return nullptr; }
	
	if (Entries.Contains(InTag)) { return nullptr; }

	ItemTagContainer.AddTag(InTag);
	
	UKRInventoryItemInstance* Instance = NewObject<UKRInventoryItemInstance>();
	
	Instance->SetItemTag(InTag);

	if (OwnerContext)
	{
		if (UWorld* World = OwnerContext->GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				Instance->SetOwnerContext(GI);
			}
		}
	}
	
	UKRInventoryItemDefinition* Def = UKRInventoryItemDefinition::CreateItemDefinition();
	Instance->SetItemDef(Def);

	Entries.Add(InTag, FKRInventoryEntry(Instance));

	return &Entries[InTag];
}

void FKRInventoryList::RemoveItem(const FGameplayTag& InTag)
{
	if (!InTag.IsValid()) { return; }
	
	if (!Entries.Contains(InTag)) { return; }
	
	ItemTagContainer.RemoveTag(InTag);
	
	Entries.Remove(InTag);
}

const FKRInventoryEntry* FKRInventoryList::AddItem(const FGameplayTag& InTag, int32 StackCount, bool& OutIsNew)
{
	if (!InTag.IsValid()) { return nullptr; }
	
	FKRInventoryEntry* Entry = Entries.Find(InTag);
	if (Entry)
	{
		Entry->AddCount(StackCount);
		OutIsNew = false;
	}
	else
	{
		Entry = CreateItem(InTag);
		Entry->AddCount(StackCount-1);
		OutIsNew = true;
	}

	return Entry;
}

void FKRInventoryList::SubtractItem(const FGameplayTag& InTag, int32 StackCount)
{
	if (!InTag.IsValid()) { return; }
	
	FKRInventoryEntry* Entry = Entries.Find(InTag);
	if (!Entry) { return; }

	Entry->SubtractCount(StackCount);
	if (Entry->GetStackCount() <= 0)
	{
		RemoveItem(InTag);
	}
}

FKRInventoryEntry* FKRInventoryList::GetItem(const FGameplayTag& InTag)
{
	check(InTag.IsValid());
	return Entries.Find(InTag);
}

const FKRInventoryEntry* FKRInventoryList::GetItem(const FGameplayTag& InTag) const
{
	check(InTag.IsValid());
	return Entries.Find(InTag);
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

TArray<UKRInventoryItemInstance*> FKRInventoryList::FindAllItemsByTag(const FGameplayTag& FilterTag) const
{
	TArray<UKRInventoryItemInstance*> FindInstances;
	if (!FilterTag.IsValid()) { return FindInstances; }
	
	for (const FGameplayTag& ItemTag : ItemTagContainer.GetGameplayTagArray())
	{
		if (ItemTag.MatchesTag(FilterTag))
		{
			if (const FKRInventoryEntry* Entry = Entries.Find(ItemTag))
			{
				if (UKRInventoryItemInstance* FindInstance = Entry->GetItemInstance())
				{
					FindInstances.Add(FindInstance);
				}
			}
		}
	}

	return FindInstances;
}

void FKRInventoryList::Clear()
{
	Entries.Empty();
}

void FKRInventoryList::AddEntryDirect(UKRInventoryItemInstance* NewInstance)
{
	if (!NewInstance || !NewInstance->GetItemTag().IsValid()) return;

	FGameplayTag Tag = NewInstance->GetItemTag();

	FKRInventoryEntry NewEntry(NewInstance, 1);

	Entries.Add(Tag, NewEntry);
	ItemTagContainer.AddTag(Tag);
}

void UKRInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PlayerInventory.SetOwnerContext(this);

	InitializeItemDefinitionFragments();
	InitializeQuickSlots();
	
	RegisterAllListeners();
}

void UKRInventorySubsystem::Deinitialize()
{
	EndListenQuickSlotAssignConfirm();
	Super::Deinitialize();
}

TSubclassOf<UKRInventoryItemFragment> UKRInventorySubsystem::GetFragmentClass(const FGameplayTag& Tag) const 
{
	if (!Tag.IsValid()) { return nullptr; }
	
	if (const TSubclassOf<UKRInventoryItemFragment>* Found = FragmentRegistry.Find(Tag)) { return *Found; }
	
	return nullptr;
}

void UKRInventorySubsystem::AddFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag)
{
	TSubclassOf<UKRInventoryItemFragment> FragmentClass = FragmentRegistry[FragmentTag];
	if (!FragmentClass) return;

	FKRInventoryEntry* Entry = GetInventory().GetItem(ItemTag);
	if (!Entry) return;

	UKRInventoryItemInstance* Instance = Entry->GetItemInstance();
	if (!Instance) return;
	
	UKRInventoryItemFragment* NewFragment = NewObject<UKRInventoryItemFragment>(Instance, FragmentClass);

	Instance->AddInstanceFragment(FragmentTag, NewFragment);

	NewFragment->OnInstanceCreated(Instance);
}

const UKRInventoryItemDefinition* UKRInventorySubsystem::GetItemDefinition(const FGameplayTag& ItemTag) const
{
	const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
	if (!Entry) { return nullptr; }

	const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
	if (!ItemInstance) { return nullptr; }

	return ItemInstance->GetItemDef();
}

const UKRInventoryItemFragment* UKRInventorySubsystem::GetItemFragment(const FGameplayTag& ItemTag, FGameplayTag FragmentTag) const
{
	const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
	if (!Entry) { return nullptr; }

	const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
	if (!ItemInstance) { return nullptr; }

	return ItemInstance->FindFragmentByTag(FragmentTag);
}

TArray<FGameplayTag> UKRInventorySubsystem::GetAllRegisteredItemTags() const
{
	TArray<FGameplayTag> OutTags;
	FragmentRegistry.GetKeys(OutTags);
	
	return OutTags;
}

UKRInventoryItemInstance* UKRInventorySubsystem::AddItem(const FGameplayTag& ItemTag, int32 StackCount)
{
	bool bIsNew = false;
	const FKRInventoryEntry* Entry = GetInventory().AddItem(ItemTag, StackCount, bIsNew);
	if (bIsNew)
	{
		InitFragment(ItemTag);
	}
	
	FAddItemMessage AddItemMessage;
	AddItemMessage.ItemTag   = ItemTag;
	AddItemMessage.StackCount= StackCount;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage<FAddItemMessage>(KRTAG_PLAYER_ACTION_ADDITEM, AddItemMessage);

	FGameplayMessageListenerHandle ListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		KRTAG_PLAYER_ACTION_ADDITEM,
		this,
		&UKRInventorySubsystem::OnMessageReceived);

	UGameplayMessageSubsystem::Get(this).UnregisterListener(ListenerHandle);
	
	UAsyncAction_ListenForGameplayMessage::ListenForGameplayMessages(this, KRTAG_PLAYER_ACTION_ADDITEM, FAddItemMessage::StaticStruct());
	
	NotifyQuickSlotQuantityChanged(ItemTag);

	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

void UKRInventorySubsystem::SubtractItem(const FGameplayTag& ItemTag, int32 StackCount)
{
	GetInventory().SubtractItem(ItemTag, StackCount);
	NotifyQuickSlotQuantityChanged(ItemTag);
}

void UKRInventorySubsystem::RemoveItem(const FGameplayTag& InTag)
{
	FGameplayMessageListenerHandle ListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
			KRTAG_PLAYER_ACTION_REMOVEITEM,
			this,
			&UKRInventorySubsystem::OnMessageReceived);

	UGameplayMessageSubsystem::Get(this).UnregisterListener(ListenerHandle);
	
	GetInventory().RemoveItem(InTag);
}

const UKRInventoryItemInstance* UKRInventorySubsystem::GetItem(const FGameplayTag& InTag)
{
	const FKRInventoryEntry* Entry = GetInventory().GetItem(InTag);
	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

int32 UKRInventorySubsystem::GetItemCountByTag(const FGameplayTag& InTag)
{
	int32 ItemQuantity = GetItemQuantity_Internal(InTag);
	
	return ItemQuantity;
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::GetAllItems() const
{
	return PlayerInventory.GetAllItems();
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::FindItemsByTag(const FGameplayTag& ItemTag) const
{
	if (!ItemTag.IsValid()) { return {}; }

	return PlayerInventory.FindAllItemsByTag(ItemTag);
}

void UKRInventorySubsystem::ClearInventory()
{
	PlayerInventory.Clear();
}

void UKRInventorySubsystem::InitializeQuickSlots()
{
	PlayerQuickSlot.Empty();
	PlayerQuickSlotStacks.Empty();
	
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_North(), FGameplayTag());
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_East(),  FGameplayTag());
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_South(), FGameplayTag());
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_West(),  FGameplayTag());

	for (const auto& Pair : PlayerQuickSlot)
	{
		PlayerQuickSlotStacks.Add(Pair.Key, 0);
	}
}

bool UKRInventorySubsystem::BindItemByQuickSlotTag(FGameplayTag SlotTag, FGameplayTag ItemTag)
{
	if (!IsValidQuickSlot(SlotTag))
		return false;

	const bool bHadAssignedBefore = IsAssignedQuickSlot();

	if (!ItemTag.IsValid())
		return HandleQuickSlotUnbind(SlotTag, ItemTag);

	UKRInventoryItemInstance* Instance = nullptr;
	if (!HasQuickSlotItemFragment(ItemTag, Instance))
		return false;

	UnbindExistingItemBySlotTag(SlotTag, ItemTag);

	PlayerQuickSlot[SlotTag] = ItemTag;

	const int32 ActualCount = GetItemQuantity_Internal(ItemTag);
	const int32 QuickSlotCount = GetQuickSlotDisplayCount(ItemTag, ActualCount);

	PlayerQuickSlotStacks.FindOrAdd(SlotTag) = QuickSlotCount;

	SendQuickSlotMessage(
		EQuickSlotAction::ItemRegistered,
		SlotTag,
		ItemTag,
		QuickSlotCount
	);

	HandleAutoSelectSlotFirstRegisteredItem(SlotTag, bHadAssignedBefore);

	return true;
}

FGameplayTag UKRInventorySubsystem::GetQuickSlotItemTag(FGameplayTag SlotTag) const
{
	if (!IsValidQuickSlot(SlotTag)) { return FGameplayTag(); }
	if (const FGameplayTag* Found = PlayerQuickSlot.Find(SlotTag)) { return *Found; }

	return FGameplayTag();
}

UKRInventoryItemInstance* UKRInventorySubsystem::GetQuickSlotItemInstance(FGameplayTag SlotTag) const
{
	return FindInstanceByItemTag(GetQuickSlotItemTag(SlotTag));
}

bool UKRInventorySubsystem::GetQuickSlotData(FGameplayTag SlotTag, FGameplayTag& OutItemTag, int32& OutQuantity) const
{
	OutItemTag  = FGameplayTag();
	OutQuantity = 0;

	if (!IsValidQuickSlot(SlotTag)) { return false; }
	
	OutItemTag = GetQuickSlotItemTag(SlotTag);

	if (!OutItemTag.IsValid()) { return false; }

	if (const int32* SlotCountPtr = PlayerQuickSlotStacks.Find(SlotTag))
	{
		OutQuantity = *SlotCountPtr;
	}
	else
	{
		OutQuantity = 0;
	}

	return true;
}

UAbilitySystemComponent* UKRInventorySubsystem::GetPlayerASC() const
{
	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (AKRPlayerState* KRPS = PC->GetPlayerState<AKRPlayerState>())
			{
				if (UAbilitySystemComponent* ASC = KRPS->GetAbilitySystemComponent())
				{
					return ASC;
				}
			}
		}
	}

	return nullptr;
}

bool UKRInventorySubsystem::UseQuickSlotItem(FGameplayTag SlotTag)
{
	if (!IsValidQuickSlot(SlotTag))
	{
		UE_LOG(LogInventorySubSystem, Warning, TEXT("UseQuickSlotItem: invalid slot %s"), *SlotTag.ToString());
		return false;
	}

	const FGameplayTag ItemTag = GetQuickSlotItemTag(SlotTag);
	if (!ItemTag.IsValid())
	{
		UE_LOG(LogInventorySubSystem, Warning, TEXT("UseQuickSlotItem: slot has no item %s"), *SlotTag.ToString());
		return false;
	}
	
	int32* SlotCountPtr = PlayerQuickSlotStacks.Find(SlotTag);
	if (!SlotCountPtr || *SlotCountPtr <= 0)
	{
		UE_LOG(LogInventorySubSystem, Warning, TEXT("UseQuickSlotItem: SlotCount<=0 Slot=%s Item=%s Stored=%d ActualInv=%d"),
			*SlotTag.ToString(), *ItemTag.ToString(),
			SlotCountPtr ? *SlotCountPtr : -1,
			GetItemQuantity_Internal(ItemTag));
		return false;
	}
    int32& SlotCount = *SlotCountPtr;
	
    int32 CurrentCount = GetItemQuantity_Internal(ItemTag);
    if (CurrentCount <= 0) { return false; }

    UKRInventoryItemInstance* Instance = FindInstanceByItemTag(ItemTag);
    if (!Instance) { return false; }

    UAbilitySystemComponent* ASC = GetPlayerASC();
    if (!ASC)
    {
        return false;
    }

    FText Reason;
    if (!CanUseConsumableItem(ItemTag, ASC, Reason))
    {
        return false;
    }

    const FGameplayTag ConsumableFragTag =
        FGameplayTag::RequestGameplayTag(TEXT("Fragment.Item.Consumable"));

    const UInventoryFragment_ConsumableItem* ConsumableFragConst =
        Cast<UInventoryFragment_ConsumableItem>(Instance->FindFragmentByTag(KRTAG_FRAGMENT_ITEM_CONSUMABLE));

    if (!ConsumableFragConst)
    {
        return false;
    }

    UInventoryFragment_ConsumableItem* ConsumableFrag =
        const_cast<UInventoryFragment_ConsumableItem*>(ConsumableFragConst);
	
    if (!ConsumableFrag->UseConsumable(ASC))
    {
        return false;
    }

    // Decrease QuickSlot count
    SlotCount = FMath::Max(0, SlotCount - 1);

    // Also subtract from inventory
    SubtractItem(ItemTag, 1);

	const float RawTimeForUI = GetConsumableUITime(ConsumableFrag);

	float TimeForUI = RawTimeForUI;
	if (SlotCount <= 0)
	{
		TimeForUI = 0.f;
	}

	// Send UI update message with QuickSlot count
	SendQuickSlotMessage(
		EQuickSlotAction::QuantityUpdated,
		SlotTag,
		ItemTag,
		SlotCount
	);

	if (SlotCount > 0)
	{
		SendQuickSlotMessage(
		EQuickSlotAction::ItemUsed,
		SlotTag,
		ItemTag,
		SlotCount,
		TimeForUI
		);
	}

	if (SlotCount <= 0 && !IsPersistentQuickSlotItem(ItemTag))
	{
		HandleQuickSlotUnbind(SlotTag, ItemTag);

		if (!SelectNextAvailableQuickSlot())
		{
			NotifySlotChanged(FGameplayTag());
		}
	}

	return true;
}

bool UKRInventorySubsystem::SelectQuickSlotClockwise()
{
	return SelectQuickSlotInternal(true);
}

bool UKRInventorySubsystem::SelectQuickSlotCounterClockwise()
{
	return SelectQuickSlotInternal(false);
}

bool UKRInventorySubsystem::CanUseConsumableItem(
    FGameplayTag ItemTag,
    UAbilitySystemComponent* TargetASC,
    FText& OutReason) const
{
    if (!TargetASC)
    {
        OutReason = FText::FromString(TEXT("Invalid target"));
        return false;
    }

    if (!ItemTag.IsValid())
    {
        OutReason = FText::FromString(TEXT("Invalid item tag"));
        return false;
    }

    const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
    if (!Entry || Entry->GetStackCount() <= 0)
    {
        OutReason = FText::FromString(TEXT("Not enough items"));
        return false;
    }

    const UInventoryFragment_ConsumableItem* ConsumableFrag = GetConsumableFragmentFromItem(Entry);
    if (!ConsumableFrag)
    {
        OutReason = FText::FromString(TEXT("Not a consumable item"));
        return false;
    }
	
    if (ConsumableFrag->IsOnCooldown(TargetASC))
    {
        const float Remaining = ConsumableFrag->GetRemainingCooldown(TargetASC);

        OutReason = FText::Format(
            FText::FromString(TEXT("Cooldown: {0}s")),
            FText::AsNumber(FMath::CeilToInt(Remaining))
        );
        return false;
    }
	
    if (!ConsumableFrag->CanApplyMoreStacks(TargetASC))
    {
        OutReason = FText::FromString(TEXT("Already in use"));
        return false;
    }

    OutReason = FText::GetEmpty();
    return true;
}

float UKRInventorySubsystem::GetConsumableCooldownRemaining(FGameplayTag ItemTag,
	UAbilitySystemComponent* TargetASC) const
{
	if (!TargetASC || !ItemTag.IsValid())
	{
		return 0.f;
	}

	const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
	if (!Entry)
	{
		return 0.f;
	}

	const UInventoryFragment_ConsumableItem* ConsumableFrag = GetConsumableFragmentFromItem(Entry);
	if (!ConsumableFrag)
	{
		return 0.f;
	}

	return ConsumableFrag->GetRemainingCooldown(TargetASC);
}

int32 UKRInventorySubsystem::GetQuickSlotDisplayCount(const FGameplayTag& ItemTag, int32 ActualCount) const
{
	if (!ItemTag.IsValid() || ActualCount <= 0)
	{
		return ActualCount;
	}

	int32 StackMax = 0;
	
	if (const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag))
	{
		if (const UInventoryFragment_ConsumableItem* Frag = GetConsumableFragmentFromItem(Entry))
		{
			StackMax = Frag->EffectConfig.StackMax;
		}
	}

	if (StackMax > 0)
	{
		return FMath::Min(ActualCount, StackMax);
	}
	
	return ActualCount;
}

bool UKRInventorySubsystem::UseConsumableItem(FGameplayTag ItemTag, UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC || !ItemTag.IsValid())
	{
		return false;
	}

	FText Reason;
	if (!CanUseConsumableItem(ItemTag, TargetASC, Reason))
	{
		return false;
	}

	FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
	if (!Entry)
	{
		return false;
	}

	UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
	if (!ItemInstance || !ItemInstance->GetItemDef())
	{
		return false;
	}

	UInventoryFragment_ConsumableItem* ConsumableFrag =
		const_cast<UInventoryFragment_ConsumableItem*>(
			Cast<UInventoryFragment_ConsumableItem>(
				ItemInstance->GetItemDef()->FindFragmentByTag(KRTAG_FRAGMENT_ITEM_CONSUMABLE)));

	if (!ConsumableFrag)
	{
		return false;
	}
	
	if (!ConsumableFrag->UseConsumable(TargetASC))
	{
		return false;
	}
	
	Entry->SubtractCount(1);
	if (Entry->GetStackCount() <= 0)
	{
		PlayerInventory.RemoveItem(ItemTag);
	}
	
	NotifyQuickSlotQuantityChanged(ItemTag);

	return true;
}

int32 UKRInventorySubsystem::GetQuickSlotItemCount(const FGameplayTag& SlotTag) const
{
	if (!IsValidQuickSlot(SlotTag)) { return 0; }

	if (const int32* SlotCountPtr = PlayerQuickSlotStacks.Find(SlotTag))
	{
		return *SlotCountPtr;
	}

	return 0;
}

void UKRInventorySubsystem::OnMessageReceived(const FGameplayTag Channel, const FAddItemMessage& Message)
{
	UE_LOG(LogInventorySubSystem, Log, TEXT("[Inventory Message] Tag:%s Item:%s Count:%d"),
		*Channel.ToString(),
		*Message.ItemTag.ToString(),
		Message.StackCount
	);
}

void UKRInventorySubsystem::AddItemInstance(UKRInventoryItemInstance* InInstance)
{
	if (!InInstance) return;

	PlayerInventory.AddEntryDirect(InInstance);
}

void UKRInventorySubsystem::BeginListenQuickSlotAssignConfirm()
{
	if (QuickSlotConfirmHandle.IsValid())
	{
		return;
	}
    
	UGameplayMessageSubsystem& MsgSys = UGameplayMessageSubsystem::Get(this);

	QuickSlotConfirmHandle = MsgSys.RegisterListener(
		FKRUIMessageTags::Confirm(),
		this,
		&UKRInventorySubsystem::OnQuickSlotConfirmMessage
	);
}

void UKRInventorySubsystem::EndListenQuickSlotAssignConfirm()
{
	if (!QuickSlotConfirmHandle.IsValid())
	{
		return;
	}

	UGameplayMessageSubsystem& MsgSys = UGameplayMessageSubsystem::Get(this);
	MsgSys.UnregisterListener(QuickSlotConfirmHandle);

	QuickSlotConfirmHandle = FGameplayMessageListenerHandle();
}

FKRInventoryList& UKRInventorySubsystem::GetInventory()
{
	return PlayerInventory;
}

void UKRInventorySubsystem::InitFragment(FGameplayTag ItemTag)
{
	UKRDataTablesSubsystem* DataTableSubsystem = GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataTableSubsystem) { return; }

	FItemDataStruct* ItemData = DataTableSubsystem->GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
	if (!ItemData) { return; }

	for (const FGameplayTag& AbilityTag : ItemData->FragmentTags)
	{
		if (!FragmentRegistry.Contains(AbilityTag))
		{
			continue;
		}
		
		AddFragment(ItemTag, AbilityTag);
	}
}

void UKRInventorySubsystem::InitializeItemDefinitionFragments()
{
	FragmentRegistry.Empty();
	
	InitialFragmentType(UInventoryFragment_EquippableItem::StaticClass());
	InitialFragmentType(UInventoryFragment_SetStats::StaticClass());
	InitialFragmentType(UInventoryFragment_QuickSlot::StaticClass());
	InitialFragmentType(UInventoryFragment_DisplayUI::StaticClass());
	InitialFragmentType(UInventoryFragment_EnhanceableItem::StaticClass());
	InitialFragmentType(UKRInventoryFragment_SellableItem::StaticClass());
	InitialFragmentType(UInventoryFragment_ConsumableItem::StaticClass());
	InitialFragmentType(UInventoryFragment_ModuleItem::StaticClass());
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

void UKRInventorySubsystem::RegisterAllListeners()
{
	UnregisterAllListeners();
	
	UGameplayMessageSubsystem& MsgSys = UGameplayMessageSubsystem::Get(this);
	
	if (ListenerHandles.Num() > 0)
	{
		return;
	}
	
	ListenerHandles.Add(
		MsgSys.RegisterListener(
			FKRUIMessageTags::Confirm(),
			this,
			&UKRInventorySubsystem::OnQuickSlotConfirmMessage
		)
	);
	
	ListenerHandles.Add(
		MsgSys.RegisterListener(
			FGameplayTag::RequestGameplayTag(FName("Player.Action.AddItem")),
			this,
			&UKRInventorySubsystem::OnMessageReceived
		)
	);

	ListenerHandles.Add(
		MsgSys.RegisterListener(
			FGameplayTag::RequestGameplayTag(FName("Player.Action.RemoveItem")),
			this,
			&UKRInventorySubsystem::OnMessageReceived
		)
	);
}

void UKRInventorySubsystem::UnregisterAllListeners()
{
	if (ListenerHandles.Num() == 0)
	{
		return;
	}

	UGameplayMessageSubsystem& MsgSys = UGameplayMessageSubsystem::Get(this);

	for (FGameplayMessageListenerHandle& Handle : ListenerHandles)
	{
		if (Handle.IsValid())
		{
			MsgSys.UnregisterListener(Handle);
		}
	}

	ListenerHandles.Reset();
}

bool UKRInventorySubsystem::IsPersistentQuickSlotItem(const FGameplayTag& ItemTag) const
{
	if (!ItemTag.IsValid()) { return false; }
	return ItemTag.MatchesTagExact(KRTAG_ITEMTYPE_CONSUME_POTION_FIRSTAID) || ItemTag.MatchesTagExact(KRTAG_ITEMTYPE_CONSUME_POTION_STAMINA) || ItemTag.MatchesTagExact(KRTAG_ITEMTYPE_CONSUME_POTION_COREBATTERY) || ItemTag.MatchesTagExact(KRTAG_ITEMTYPE_CONSUME_INSURANCE_COIN) || ItemTag.MatchesTagExact(KRTAG_ITEMTYPE_CONSUME_INSURANCE_WALLET);
}

bool UKRInventorySubsystem::IsAssignedQuickSlot() const
{
	bool bFound = false;

	ForEachQuickSlot(PlayerQuickSlot,
		[&](const FGameplayTag&, const FGameplayTag& ItemTag)
		{
			if (ItemTag.IsValid()) { bFound = true; }
		});

	return bFound;
}

bool UKRInventorySubsystem::IsAssignedSelectedQuickSlot() const
{
    bool bSelectable = false;

    ForEachQuickSlot(PlayerQuickSlot,
        [&](const FGameplayTag& SlotTag, const FGameplayTag& ItemTag)
        {
            if (!ItemTag.IsValid()) { return; }
            if (GetQuickSlotItemDisplayCount(ItemTag) > 0)
            {
                bSelectable = true;
            }
        });

    return bSelectable;
}

bool UKRInventorySubsystem::SelectQuickSlotInternal(bool bClockwise)
{
	if (!IsAssignedSelectedQuickSlot())
	{
		NotifySlotChanged(FGameplayTag());
		return false;
	}

	const TArray<FGameplayTag>& SlotOrder = GetQuickSlotOrder();

	auto FindSlotIndex = [&SlotOrder](const FGameplayTag& InSlotTag) -> int32
	{
		return SlotOrder.IndexOfByKey(InSlotTag);
	};

	int32 StartIndex = FindSlotIndex(SelectedQuickSlot);
	if (StartIndex == INDEX_NONE)
	{
		StartIndex = -1;
	}

	const int32 Dir = bClockwise ? 1 : -1;
	const int32 NumSlots = SlotOrder.Num();

	for (int32 Step = 1; Step <= NumSlots; ++Step)
	{
		const int32 Index = (StartIndex + Dir * Step + NumSlots) % NumSlots;
		const FGameplayTag& CandidateSlot = SlotOrder[Index];

		const FGameplayTag ItemTag = GetQuickSlotItemTag(CandidateSlot);
		if (!ItemTag.IsValid()) { continue; }

		const int32 Count = GetQuickSlotItemDisplayCount(ItemTag);
		if (Count <= 0) { continue; }

		SelectedQuickSlot = CandidateSlot;
		NotifySlotChanged(SelectedQuickSlot);
		return true;
	}

	NotifySlotChanged(FGameplayTag());
	return false;
}

void UKRInventorySubsystem::NotifySlotChanged(const FGameplayTag& NewSelectedSlot)
{
	SelectedQuickSlot = NewSelectedSlot;

	FGameplayTag ItemTag;
	int32 ItemCount = 0;

	if (NewSelectedSlot.IsValid())
	{
		ItemTag = GetQuickSlotItemTag(NewSelectedSlot);

		if (const int32* SlotCountPtr = PlayerQuickSlotStacks.Find(NewSelectedSlot))
		{
			ItemCount = *SlotCountPtr;
		}
	}

	SendQuickSlotMessage(
		EQuickSlotAction::SlotChanged,
		NewSelectedSlot,
		ItemTag,
		ItemCount
	);
}

FGameplayTag UKRInventorySubsystem::MapSlotTagToUISlotTag(const FGameplayTag& SlotTag) const
{
	return SlotTag;
}

void UKRInventorySubsystem::SendQuickSlotMessage(
	EQuickSlotAction ActionType,
	const FGameplayTag& SlotTag,
	const FGameplayTag& ItemTag,
	int32 ItemQuantity,
	float DurationSeconds
)
{
	FKRUIMessage_QuickSlot Payload;
	Payload.ActionType           = ActionType;
	Payload.ItemQuantity         = ItemQuantity;
	Payload.Duration             = (ActionType == EQuickSlotAction::ItemUsed) ? DurationSeconds : 0.f;
	Payload.CurrentlySelectedSlot= MapSlotTagToUISlotTag(SlotTag);
	
	if (ItemTag.IsValid())
	{
		const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
		if (Entry)
		{
			if (UKRInventoryItemInstance* Instance = Entry->GetItemInstance())
			{
				const UKRInventoryItemFragment* DisplayFragBase =
					Instance->FindFragmentByTag(KRTAG_ABILITY_ITEM_DISPLAYUI);

				if (const UInventoryFragment_DisplayUI* DisplayFrag =
					Cast<UInventoryFragment_DisplayUI>(DisplayFragBase))
				{
					Payload.ItemNameKey = DisplayFrag->DisplayNameKey;
					Payload.ItemIcon    = DisplayFrag->ItemIcon;
				}
			}
		}
	}

	const FGameplayTag Channel = FKRUIMessageTags::QuickSlot();

	UGameplayMessageSubsystem::Get(this)
		.BroadcastMessage<FKRUIMessage_QuickSlot>(Channel, Payload);
}

bool UKRInventorySubsystem::SelectNextAvailableQuickSlot()
{
	FGameplayTag FoundSlot;

	ForEachQuickSlot(PlayerQuickSlot,
		[&](const FGameplayTag& SlotTag, const FGameplayTag& BoundTag)
		{
			if (!FoundSlot.IsValid() && BoundTag.IsValid())
			{
				int32 Count = GetQuickSlotItemDisplayCount(BoundTag);
				if (Count > 0)
				{
					FoundSlot = SlotTag;
				}
			}
		});

	if (FoundSlot.IsValid())
	{
		SelectedQuickSlot = FoundSlot;
		NotifySlotChanged(FoundSlot);
		return true;
	}

	return false;
}

void UKRInventorySubsystem::NotifyQuickSlotQuantityChanged(const FGameplayTag& ItemTag)
{
	if (!ItemTag.IsValid()) { return; }

	const int32 ActualInventoryCount = GetItemQuantity_Internal(ItemTag);

	// Only handle complete item removal from inventory
	if (ActualInventoryCount <= 0)
	{
		ForEachQuickSlot(PlayerQuickSlot,
			[&](const FGameplayTag& SlotTag, FGameplayTag& BoundItemTag)
			{
				if (BoundItemTag != ItemTag) { return; }

				// Item completely removed from inventory, unbind from quickslot
				if (!IsPersistentQuickSlotItem(ItemTag))
				{
					PlayerQuickSlotStacks.FindOrAdd(SlotTag) = 0;
					HandleQuickSlotUnbind(SlotTag, ItemTag);
				}
			});

		const bool bHasAnyBound = IsAssignedQuickSlot();
		const bool bHasAnySelectable = IsAssignedSelectedQuickSlot();
		if (!bHasAnyBound || !bHasAnySelectable)
		{
			NotifySlotChanged(FGameplayTag());
		}

		if (bHasAnyBound)
		{
			SelectNextAvailableQuickSlot();
		}
	}
	// If item still exists in inventory, don't update QuickSlot count
	// QuickSlot count is managed independently
}

bool UKRInventorySubsystem::IsValidQuickSlot(const FGameplayTag& SlotTag) const
{
	return SlotTag.IsValid() && PlayerQuickSlot.Contains(SlotTag);
}

bool UKRInventorySubsystem::HasQuickSlotItemFragment(FGameplayTag ItemTag, UKRInventoryItemInstance*& OutInstance)
{
	OutInstance = FindInstanceByItemTag(ItemTag);
	if (!OutInstance)
		return false;
	return OutInstance->FindFragmentByTag(KRTAG_ABILITY_ITEM_QUICKSLOT) != nullptr;
}

const FKRInventoryEntry* UKRInventorySubsystem::FindEntryByItemTag(const FGameplayTag& ItemTag) const
{
	if (!ItemTag.IsValid()) { return nullptr; }
	
	return PlayerInventory.GetItem(ItemTag);
}

UKRInventoryItemInstance* UKRInventorySubsystem::FindInstanceByItemTag(const FGameplayTag& ItemTag) const
{
	const FKRInventoryEntry* Entry = FindEntryByItemTag(ItemTag);
	return Entry ? Entry->GetItemInstance() : nullptr;
}

bool UKRInventorySubsystem::HandleQuickSlotUnbind(FGameplayTag SlotTag, FGameplayTag ItemTag)
{
	if (!IsValidQuickSlot(SlotTag)) { return false; }

	if (ItemTag.IsValid())
	{
		FGameplayTag& UnboundItem = PlayerQuickSlot[SlotTag];
		if (!UnboundItem.IsValid()) { return false; }

		UnboundItem = FGameplayTag();

		SendQuickSlotMessage(
				EQuickSlotAction::ItemUsed,
				SlotTag,
				ItemTag,
				0,
				0.1f
				);
        
		SendQuickSlotMessage(
			EQuickSlotAction::ItemUnregistered,
			SlotTag,
			ItemTag,
			0
		);
	}

	if (!IsAssignedQuickSlot())
	{
		NotifySlotChanged(FGameplayTag());
	}

	return true;
}

void UKRInventorySubsystem::UnbindExistingItemBySlotTag(FGameplayTag SlotTag, FGameplayTag ItemTag)
{
	ForEachQuickSlot(PlayerQuickSlot,
	   [&](const FGameplayTag& OtherSlot, FGameplayTag& OtherItemTag)
	   {
		   if (OtherSlot == SlotTag)
		   	return;

		   if (OtherItemTag == ItemTag)
		   	HandleQuickSlotUnbind(OtherSlot, ItemTag);
	   });
}

void UKRInventorySubsystem::HandleAutoSelectSlotFirstRegisteredItem(FGameplayTag SlotTag, bool bHadAssignedBefore)
{
	if (!bHadAssignedBefore && !SelectedQuickSlot.IsValid())
	{
		SelectedQuickSlot = SlotTag;
		NotifySlotChanged(SlotTag);
	}
}

int32 UKRInventorySubsystem::GetItemQuantity_Internal(const FGameplayTag& ItemTag) const
{
	if (!ItemTag.IsValid()) { return 0; }

	if (const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag))
	{
		return Entry->GetStackCount();
	}

	return 0;
}

int32 UKRInventorySubsystem::GetQuickSlotItemDisplayCount(const FGameplayTag& ItemTag) const
{
	if (!ItemTag.IsValid()) { return 0; }

	// Check if this item is bound to any quick slot
	for (const auto& Pair : PlayerQuickSlot)
	{
		const FGameplayTag& SlotTag = Pair.Key;
		const FGameplayTag& BoundTag = Pair.Value;

		if (BoundTag != ItemTag) { continue; }

		// Found the item, return the count from PlayerQuickSlotStacks
		if (const int32* SlotCountPtr = PlayerQuickSlotStacks.Find(SlotTag))
		{
			return *SlotCountPtr;
		}
	}

	return 0;
}

int32 UKRInventorySubsystem::GetConsumableStackMaxForItem(const FGameplayTag& ItemTag) const
{
	if (!ItemTag.IsValid())
	{
		return 0;
	}

	int32 StackMax = 0;

	// 1) 우선 Consumable Fragment에서 StackMax 가져오기
	if (const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag))
	{
		if (const UInventoryFragment_ConsumableItem* Frag = GetConsumableFragmentFromItem(Entry))
		{
			if (Frag->EffectConfig.StackMax > 0)
			{
				StackMax = Frag->EffectConfig.StackMax;
			}
		}
	}
	
	return StackMax;
}

void UKRInventorySubsystem::OnQuickSlotConfirmMessage(FGameplayTag Channel, const FKRUIMessage_Confirm& Payload)
{
    if (Payload.Context == EConfirmContext::QuickSlotAssign)
    {
        EndListenQuickSlotAssignConfirm();
        
        if (Payload.Result != EConfirmResult::Yes)
        {
            return;
        }
        
        BindItemByQuickSlotTag(Payload.SlotTag, Payload.ItemTag);
    }
}
