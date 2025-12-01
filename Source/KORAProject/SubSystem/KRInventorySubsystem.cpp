#include "Subsystem/KRInventorySubsystem.h"

#include "AbilitySystemInterface.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Data/CacheDataTable.h"
#include "Data/ItemDataStruct.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/AsyncAction_ListenForGameplayMessage.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "Inventory/Fragment/InventoryFragment_DisplayUI.h"
#include "Inventory/Fragment/InventoryFragment_SellableItem.h"
#include "Inventory/Fragment/InventoryFragment_ConsumableItem.h"
#include "StructUtils/PropertyBag.h"
#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "Inventory/Fragment/InventoryFragment_QuickSlot.h"

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

	static const UInventoryFragment_ConsumableItem* GetConsumableFragmentFromItem(
		const FKRInventoryEntry* Entry)
	{
		if (!Entry)
		{
			return nullptr;
		}

		const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
		if (!ItemInstance || !ItemInstance->GetItemDef())
		{
			return nullptr;
		}

		const FGameplayTag ConsumableFragTag =
			FGameplayTag::RequestGameplayTag(TEXT("Fragment.Item.Consumable"));

		const UKRInventoryItemFragment* BaseFrag =
			ItemInstance->GetItemDef()->FindFragmentByTag(ConsumableFragTag);

		return Cast<UInventoryFragment_ConsumableItem>(BaseFrag);
	}
}

FKRInventoryEntry* FKRInventoryList::CreateItem(FGameplayTag InTag)
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

void FKRInventoryList::RemoveItem(FGameplayTag InTag)
{
	if (!InTag.IsValid()) { return; }
	
	if (!Entries.Contains(InTag)) { return; }
	
	ItemTagContainer.RemoveTag(InTag);
	
	Entries.Remove(InTag);
}

const FKRInventoryEntry* FKRInventoryList::AddItem(FGameplayTag InTag, int32 StackCount, bool& OutIsNew)
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

void FKRInventoryList::SubtractItem(FGameplayTag InTag, int32 StackCount)
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

FKRInventoryEntry* FKRInventoryList::GetItem(FGameplayTag InTag)
{
	check(InTag.IsValid());
	return Entries.Find(InTag);
}

const FKRInventoryEntry* FKRInventoryList::GetItem(FGameplayTag InTag) const
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

TArray<UKRInventoryItemInstance*> FKRInventoryList::FindAllItemsByTag(FGameplayTag FilterTag) const
{
	TArray<UKRInventoryItemInstance*> FindInstances;
	if (!ItemTagContainer.IsValid()) { return FindInstances; }

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

	PlayerInventory.SetOwnerContext(this);

	InitializeItemDefinitionFragments();
	InitializeQuickSlots();
}

void UKRInventorySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TSubclassOf<UKRInventoryItemFragment> UKRInventorySubsystem::GetFragmentClass(FGameplayTag Tag) const 
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
	
	Instance->GetItemDef()->AddFragment(FragmentTag, NewFragment);
	
	NewFragment->OnInstanceCreated(Instance);
}

const UKRInventoryItemDefinition* UKRInventorySubsystem::GetItemDefinition(FGameplayTag ItemTag) const
{
	const FKRInventoryEntry* Entry = PlayerInventory.GetItem(ItemTag);
	if (!Entry) { return nullptr; }

	const UKRInventoryItemInstance* ItemInstance = Entry->GetItemInstance();
	if (!ItemInstance) { return nullptr; }

	return ItemInstance->GetItemDef();
}

const UKRInventoryItemFragment* UKRInventorySubsystem::GetItemFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag) const
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

UKRInventoryItemInstance* UKRInventorySubsystem::AddItem(FGameplayTag ItemTag, int32 StackCount)
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
	
	FGameplayTag AddItemTag = FGameplayTag::RequestGameplayTag(FName("Player.Action.AddItem"));
	
	UGameplayMessageSubsystem::Get(this).BroadcastMessage<FAddItemMessage>(AddItemTag, AddItemMessage);

	FGameplayMessageListenerHandle ListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		AddItemTag,
		this,
		&UKRInventorySubsystem::OnMessageReceived);

	UGameplayMessageSubsystem::Get(this).UnregisterListener(ListenerHandle);
	
	UAsyncAction_ListenForGameplayMessage::ListenForGameplayMessages(this, AddItemTag, FAddItemMessage::StaticStruct());
	
	NotifyQuickSlotQuantityChanged(ItemTag);

	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

void UKRInventorySubsystem::SubtractItem(FGameplayTag ItemTag, int32 StackCount)
{
	GetInventory().SubtractItem(ItemTag, StackCount);
	NotifyQuickSlotQuantityChanged(ItemTag);
}

void UKRInventorySubsystem::RemoveItem(FGameplayTag InTag)
{
	FGameplayTag RemoveItemTag = FGameplayTag::RequestGameplayTag(FName("Player.Action.RemoveItem"));
	
	FGameplayMessageListenerHandle ListenerHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
			RemoveItemTag,
			this,
			&UKRInventorySubsystem::OnMessageReceived);

	UGameplayMessageSubsystem::Get(this).UnregisterListener(ListenerHandle);
	
	GetInventory().RemoveItem(InTag);
}

const UKRInventoryItemInstance* UKRInventorySubsystem::GetItem(FGameplayTag InTag)
{
	const FKRInventoryEntry* Entry = GetInventory().GetItem(InTag);
	return Entry != nullptr ? Entry->GetItemInstance() : nullptr;
}

int32 UKRInventorySubsystem::GetItemCountByTag(FGameplayTag InTag)
{
	return GetItemQuantity_Internal(InTag);
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::GetAllItems() const
{
	return PlayerInventory.GetAllItems();
}

TArray<UKRInventoryItemInstance*> UKRInventorySubsystem::FindItemsByTag(FGameplayTag ItemTag) const
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
	
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_North(), FGameplayTag());
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_East(),  FGameplayTag());
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_South(), FGameplayTag());
	PlayerQuickSlot.Add(FKRUIMessageTags::QuickSlot_West(),  FGameplayTag());
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
	
	UnbindExistingItemBySlotTag(ItemTag, SlotTag);
	
	PlayerQuickSlot[SlotTag] = ItemTag;

	const int32 NewQuantity = GetItemQuantity_Internal(ItemTag);

	SendQuickSlotMessage(
		EQuickSlotAction::ItemRegistered,
		SlotTag,
		ItemTag,
		NewQuantity
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
	
	OutQuantity = GetItemQuantity_Internal(OutItemTag);
	
	return true;
}

UAbilitySystemComponent* UKRInventorySubsystem::GetPlayerASC() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return nullptr;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return nullptr;
	}

	// 캐릭터가 IAbilitySystemInterface 구현했다면 이게 제일 정석
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn))
	{
		return ASI->GetAbilitySystemComponent();
	}

	// KR 전용 ASC 컴포넌트
	if (UKRAbilitySystemComponent* KRASC = Pawn->FindComponentByClass<UKRAbilitySystemComponent>())
	{
		return KRASC;
	}

	// 일반 ASC라도 있으면 사용
	if (UAbilitySystemComponent* ASC = Pawn->FindComponentByClass<UAbilitySystemComponent>())
	{
		return ASC;
	}

	return nullptr;
}


// CurrentSelectedSlot 을 사용하여서 구현되게 수정 (매개변수 없이 돌아가게)
bool UKRInventorySubsystem::UseQuickSlotItem(FGameplayTag SlotTag)
{
	if (!IsValidQuickSlot(SlotTag)) { return false; }

	const FGameplayTag ItemTag = GetQuickSlotItemTag(SlotTag);
	if (!ItemTag.IsValid()) { return false; }

	int32 CurrentCount = GetItemQuantity_Internal(ItemTag);
	if (CurrentCount <= 0) { return false; }

	UKRInventoryItemInstance* Instance = FindInstanceByItemTag(ItemTag);
	if (!Instance) { return false; }

	// 1) ASC 찾기
	UAbilitySystemComponent* ASC = GetPlayerASC();
	if (!ASC)
	{
		UE_LOG(LogInventorySubSystem, Warning, TEXT("UseQuickSlotItem: ASC not found"));
		return false;
	}

	// 2) CanUse 체크 (쿨다운 / InUse / 개수)
	FText Reason;
	if (!CanUseConsumableItem(ItemTag, ASC, Reason))
	{
		UE_LOG(LogInventorySubSystem, Log,
			TEXT("UseQuickSlotItem blocked: %s"), *Reason.ToString());
		return false;
	}

	// 3) Consumable Fragment
	const FGameplayTag ConsumableFragTag =
		FGameplayTag::RequestGameplayTag(TEXT("Fragment.Item.Consumable"));

	const UInventoryFragment_ConsumableItem* ConsumableFragConst =
		Cast<UInventoryFragment_ConsumableItem>(Instance->FindFragmentByTag(ConsumableFragTag));

	if (!ConsumableFragConst)
	{
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("UseQuickSlotItem: Item %s has no Consumable fragment"), *ItemTag.ToString());
		return false;
	}

	UInventoryFragment_ConsumableItem* ConsumableFrag =
		const_cast<UInventoryFragment_ConsumableItem*>(ConsumableFragConst);

	// 4) 실제 사용
	if (!ConsumableFrag->UseConsumable(ASC))
	{
		return false;
	}

	// 5) 인벤토리 수량 감소 (이 함수는 NotifyQuickSlotQuantityChanged도 호출)
	SubtractItem(ItemTag, 1);

	const int32 NewQuantity = GetItemQuantity_Internal(ItemTag);

	// 6) UI 메시지
	const float UseDuration = ConsumableFrag->EffectConfig.Duration;

	SendQuickSlotMessage(
		EQuickSlotAction::ItemUsed,
		SlotTag,
		ItemTag,
		NewQuantity,
		UseDuration
	);

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

bool UKRInventorySubsystem::CanUseConsumableItem(FGameplayTag ItemTag, UAbilitySystemComponent* TargetASC,
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

	// 쿨다운 체크
	if (ConsumableFrag->IsOnCooldown(TargetASC))
	{
		const float Remaining = ConsumableFrag->GetRemainingCooldown(TargetASC);

		OutReason = FText::Format(
			FText::FromString(TEXT("Cooldown: {0}s")),
			FText::AsNumber(FMath::CeilToInt(Remaining))
		);
		return false;
	}

	// Duration (InUse) 체크
	if (ConsumableFrag->IsInUse(TargetASC))
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

bool UKRInventorySubsystem::UseConsumableItem(FGameplayTag ItemTag, UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC || !ItemTag.IsValid())
	{
		return false;
	}

	FText Reason;
	if (!CanUseConsumableItem(ItemTag, TargetASC, Reason))
	{
		// 여기서 Reason을 UI에 넘기고 싶으면, 별도 메시지 브로드캐스트를 만들거나
		// BP에서 CanUse → Reason 확인 → 실패 시 Use 호출 안 하는 패턴으로 써도 됨.
		UE_LOG(LogInventorySubSystem, Warning, TEXT("UseConsumableItem blocked: %s"), *Reason.ToString());
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

	const FGameplayTag ConsumableFragTag =
		FGameplayTag::RequestGameplayTag(TEXT("Fragment.Item.Consumable"));

	UInventoryFragment_ConsumableItem* ConsumableFrag =
		const_cast<UInventoryFragment_ConsumableItem*>(
			Cast<UInventoryFragment_ConsumableItem>(
				ItemInstance->GetItemDef()->FindFragmentByTag(ConsumableFragTag)));

	if (!ConsumableFrag)
	{
		return false;
	}

	// 실제 GE + 쿨다운 적용
	if (!ConsumableFrag->UseConsumable(TargetASC))
	{
		return false;
	}

	// 스택 감소
	Entry->SubtractCount(1);
	if (Entry->GetStackCount() <= 0)
	{
		// 수량이 0이면 인벤토리에서 제거
		PlayerInventory.RemoveItem(ItemTag);
	}

	// 퀵슬롯 및 UI 업데이트
	NotifyQuickSlotQuantityChanged(ItemTag);

	return true;
}

void UKRInventorySubsystem::OnMessageReceived(const FGameplayTag Channel, const FAddItemMessage& Message)
{
	UE_LOG(LogInventorySubSystem, Log, TEXT("[Inventory Message] Tag:%s Item:%s Count:%d"),
		*Channel.ToString(),
		*Message.ItemTag.ToString(),
		Message.StackCount
	);
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

	for (const FGameplayTag& AbilityTag : ItemData->AbilityTags)
	{
		if (!FragmentRegistry.Contains(AbilityTag))
		{
			UE_LOG(LogInventorySubSystem, Warning,
				TEXT("No exist %s Tag. You must create this tag Fragment"),
				*AbilityTag.GetTagName().ToString());
				
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

bool UKRInventorySubsystem::IsPersistentQuickSlotItem(const FGameplayTag& ItemTag) const
{
	if (!ItemTag.IsValid()) { return false; }
	
	static const FGameplayTag Tag_FirstAid    = FGameplayTag::RequestGameplayTag(TEXT("ItemType.Consume.Potion.FirstAid"));
	static const FGameplayTag Tag_Stamina     = FGameplayTag::RequestGameplayTag(TEXT("ItemType.Consume.Potion.Stamina"));
	static const FGameplayTag Tag_CoreBattery = FGameplayTag::RequestGameplayTag(TEXT("ItemType.Consume.Potion.CoreBattery"));

	const bool bPersistent = ItemTag.MatchesTagExact(Tag_FirstAid) || ItemTag.MatchesTagExact(Tag_Stamina) || ItemTag.MatchesTagExact(Tag_CoreBattery);

	return bPersistent;
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
		[&](const FGameplayTag&, const FGameplayTag& ItemTag)
		{
			if (!ItemTag.IsValid()) { return; }

			if (GetItemQuantity_Internal(ItemTag) > 0) { bSelectable = true; }
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

		const int32 Count = GetItemQuantity_Internal(ItemTag);
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
		ItemTag   = GetQuickSlotItemTag(NewSelectedSlot);
		ItemCount = GetItemQuantity_Internal(ItemTag);
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
				const FGameplayTag DisplayUITag =
					FGameplayTag::RequestGameplayTag("Ability.Item.DisplayUI");

				const UKRInventoryItemFragment* DisplayFragBase =
					Instance->FindFragmentByTag(DisplayUITag);

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
				int32 Count = GetItemQuantity_Internal(BoundTag);
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

	const int32 NewQuantity = GetItemQuantity_Internal(ItemTag);

	FGameplayTag SlotWithThisItem;
	bool bHadAnySlot = false;
	
	ForEachQuickSlot(PlayerQuickSlot,
		[&](const FGameplayTag& SlotTag, FGameplayTag& BoundItemTag)
		{
			if (BoundItemTag != ItemTag) { return; }

			bHadAnySlot = true;
			
			if (!SlotWithThisItem.IsValid())
			{
				SlotWithThisItem = SlotTag;
			}
			
			if (NewQuantity <= 0 && !IsPersistentQuickSlotItem(ItemTag))
			{
				HandleQuickSlotUnbind(SlotTag, ItemTag);
			}
			
			SendQuickSlotMessage(
				EQuickSlotAction::QuantityUpdated,
				SlotTag,
				ItemTag,
				NewQuantity
			);
		});
	
	if (NewQuantity <= 0)
	{
		const bool bHasAnyBound      = IsAssignedQuickSlot();
		const bool bHasAnySelectable = IsAssignedSelectedQuickSlot();

		if (!bHasAnyBound || !bHasAnySelectable)
		{
			NotifySlotChanged(FGameplayTag());
		}
		
		if (bHasAnyBound)
		{
			SelectNextAvailableQuickSlot();
		}

		return;
	}
	
	if (!SelectedQuickSlot.IsValid() && SlotWithThisItem.IsValid())
	{
		SelectedQuickSlot = SlotWithThisItem;
		NotifySlotChanged(SlotWithThisItem);
	}
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
	
	const FGameplayTag QuickSlotFragTag = 
		FGameplayTag::RequestGameplayTag("Ability.Item.QuickSlot");

	return OutInstance->FindFragmentByTag(QuickSlotFragTag) != nullptr;
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

void UKRInventorySubsystem::OnConfirmMessage(FGameplayTag MessageTag, const FKRUIMessage_Confirm& Payload)
{
	if (Payload.Context == EConfirmContext::QuickSlotAssign)
	{
		BindItemByQuickSlotTag(Payload.ItemTag, Payload.SlotTag);
	}
	else if (Payload.Context == EConfirmContext::InventoryItemUse)
	{
		UAbilitySystemComponent* ASC = GetPlayerASC();
		if (!ASC)
		{
			return;
		}

		UseConsumableItem(Payload.ItemTag, ASC);
	}
}
