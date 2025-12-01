#include "KREquipmentManagerComponent.h"
#include "KREquipmentDefinition.h"
#include "KREquipmentInstance.h"
#include "Weapons/KRWeaponInstance.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentManagerComponent)

FKRAppliedEquipmentEntry* FKREquipmentList::AddEntry(TSubclassOf<UKREquipmentDefinition> InEquipmentDefinition)
{
	check(InEquipmentDefinition != nullptr);
 	check(OwnerComponent);
	
	const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(InEquipmentDefinition);

	TSubclassOf<UKREquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UKREquipmentInstance::StaticClass();
	}
	
	FKRAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = InEquipmentDefinition;
	NewEntry.Instance = NewObject<UKREquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	
	return &NewEntry;
}

void FKREquipmentList::RemoveEntry(UKREquipmentInstance* InInstance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FKRAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == InInstance)
		{
			EntryIt.RemoveCurrent();
			return;
		}
	}
}

FKRAppliedEquipmentEntry* FKREquipmentList::FindEntryByInstance(UKREquipmentInstance* InInstance)
{
	if (InInstance)
	{
		for (FKRAppliedEquipmentEntry& Entry : Entries)
		{
			if (Entry.Instance == InInstance)
			{
				return &Entry;
			}
		}
	}
	return nullptr;
}


UKREquipmentManagerComponent::UKREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	bWantsInitializeComponent = true;
}

UAbilitySystemComponent* UKREquipmentManagerComponent::GetAbilitySystemComponent() const
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
		{
			return ASI->GetAbilitySystemComponent();
		}
	}
	return nullptr;
}

UKREquipmentInstance* UKREquipmentManagerComponent::EquipItem(TSubclassOf<UKREquipmentDefinition> InEquipmentDefinition)
{
	if (InEquipmentDefinition == nullptr)
	{
		return nullptr;
	}

	const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(InEquipmentDefinition);

	const FGameplayTag SlotTagToOccupy = EquipmentCDO->EquipmentSlotTag;
	if (SlotTagToOccupy.IsValid())
	{
		TArray<UKREquipmentInstance*> OldInstancesToUnequip;

		if (UKREquipmentInstance* OldInstance = EquippedSlotsMap.FindRef(SlotTagToOccupy))
		{
			OldInstancesToUnequip.AddUnique(OldInstance);
		}

		for (UKREquipmentInstance* OldInstance : OldInstancesToUnequip)
		{
			UnequipItem(OldInstance);
		}
	}

	FKRAppliedEquipmentEntry* NewEntry = EquipmentList.AddEntry(InEquipmentDefinition);
	UKREquipmentInstance* NewInstance = NewEntry->Instance;
	
	if (NewInstance != nullptr)
	{
		NewInstance->OnEquipped(EquipmentCDO->ActorsToSpawn);
		
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant = EquipmentCDO->AbilitiesToGrant;
	
			for (const TSubclassOf<UGameplayAbility>& AbilityClass : AbilitiesToGrant)
			{
				if (AbilityClass)
				{
					FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, -1, NewInstance);
					FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(AbilitySpec);
					NewEntry->GrantedAbilityHandles.Add(AbilityHandle);
				}
			}
		}
		
		EquippedSlotsMap.Add(SlotTagToOccupy, NewInstance);
	}
	
	return NewInstance;
}

UKREquipmentInstance* UKREquipmentManagerComponent::EquipFromInventory(UKRInventoryItemInstance* ItemInstance)
{
    if (!ItemInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[Equipment] Invalid ItemInstance"));
        return nullptr;
    }

    const UKRInventoryItemDefinition* ItemDef = ItemInstance->GetItemDef();
    if (!ItemDef)
    {
        UE_LOG(LogTemp, Error, TEXT("[Equipment] No ItemDefinition"));
        return nullptr;
    }

    // Equippable Fragment 가져오기
    const UInventoryFragment_EquippableItem* EquipFragment =
        ItemDef->FindFragmentByTag<UInventoryFragment_EquippableItem>(
            FGameplayTag::RequestGameplayTag("Ability.Item.Equippable"));

    if (!EquipFragment || !EquipFragment->EquipmentDefinition)
    {
        UE_LOG(LogTemp, Error, TEXT("[Equipment] Not Equipable Item."));
        return nullptr;
    }

    TSubclassOf<UKREquipmentDefinition> EquipmentDefinition = EquipFragment->EquipmentDefinition;
    const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(EquipmentDefinition);

    // 기존 슬롯 정리
    const FGameplayTag SlotTagToOccupy = EquipmentCDO->EquipmentSlotTag;
    if (SlotTagToOccupy.IsValid())
    {
        if (UKREquipmentInstance* OldInstance = EquippedSlotsMap.FindRef(SlotTagToOccupy))
        {
            UnequipItem(OldInstance);
        }
    }

    // 인스턴스 생성
    FKRAppliedEquipmentEntry* NewEntry = EquipmentList.AddEntry(EquipmentDefinition);
    UKREquipmentInstance* NewInstance = NewEntry->Instance;

    if (!NewInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[Equipment] Failed to create instance"));
        return nullptr;
    }
    
    // Stats Fragment 적용
    const UInventoryFragment_SetStats* StatsFragment =
        ItemDef->FindFragmentByTag<UInventoryFragment_SetStats>(
            FGameplayTag::RequestGameplayTag("Ability.Item.SetStat"));

    if (StatsFragment)
    {
        NewInstance->InitializeStats(StatsFragment);
    }

    // 강화 Fragment 적용
    const UInventoryFragment_EnhanceableItem* EnhanceFragment =
        ItemDef->FindFragmentByTag<UInventoryFragment_EnhanceableItem>(
            FGameplayTag::RequestGameplayTag("Fragment.Item.Enhanceable"));

    if (EnhanceFragment)
    {
        NewInstance->ApplyEnhanceLevel(EnhanceFragment->EnhanceLevel);
    }
	
    // 액터 스폰 및 장착
    NewInstance->OnEquipped(EquipmentCDO->ActorsToSpawn);
	
    // GAS 능력 부여
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant = EquipmentCDO->AbilitiesToGrant;

        for (const TSubclassOf<UGameplayAbility>& AbilityClass : AbilitiesToGrant)
        {
            if (AbilityClass)
            {
                FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, -1, NewInstance);
                FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(AbilitySpec);
                NewEntry->GrantedAbilityHandles.Add(AbilityHandle);
            }
        }
    }

    // 슬롯 맵 업데이트
    if (SlotTagToOccupy.IsValid())
    {
        EquippedSlotsMap.Add(SlotTagToOccupy, NewInstance);
    }

    return NewInstance;
}


void UKREquipmentManagerComponent::UnequipItem(UKREquipmentInstance* InItemInstance)
{
	if (InItemInstance == nullptr)
	{
		return;
	}
	
	FKRAppliedEquipmentEntry* Entry = EquipmentList.FindEntryByInstance(InItemInstance);
	if (Entry == nullptr)
	{
		return;
	}

	const TSubclassOf<UKREquipmentDefinition> EquipmentDefClass = Entry->EquipmentDefinition;
	const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(EquipmentDefClass);
	
	InItemInstance->OnUnequipped();
	
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (const FGameplayAbilitySpecHandle& AbilityHandle : Entry->GrantedAbilityHandles)
		{
			ASC->ClearAbility(AbilityHandle);
		}
		Entry->GrantedAbilityHandles.Empty();
	}

	if (EquipmentCDO)
	{
		const FGameplayTag& SlotTag = EquipmentCDO->EquipmentSlotTag;
		
		if (EquippedSlotsMap.FindRef(SlotTag) == InItemInstance)
		{
			EquippedSlotsMap.Remove(SlotTag);
		}
	}

	EquipmentList.RemoveEntry(InItemInstance);
}

void UKREquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UKREquipmentManagerComponent::UninitializeComponent()
{
	TArray<UKREquipmentInstance*> AllEquipmentInstances;
	
	for (const FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UKREquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

UKREquipmentInstance* UKREquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UKREquipmentInstance> InstanceType) const
{
	for (const FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UKREquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UKREquipmentInstance*> UKREquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UKREquipmentInstance> InstanceType) const
{
	TArray<UKREquipmentInstance*> Results;
	for (const FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UKREquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}