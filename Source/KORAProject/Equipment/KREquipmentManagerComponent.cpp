#include "KREquipmentManagerComponent.h"
#include "KREquipmentDefinition.h"
#include "KREquipmentInstance.h"
#include "Weapons/KRWeaponInstance.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "GameFramework/Character.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentManagerComponent)

DEFINE_LOG_CATEGORY(LogEquipmentManagerComponent);

static FKRAppliedEquipmentEntry Invalid_Entry = FKRAppliedEquipmentEntry();

bool FKRAppliedEquipmentEntry::IsValid() const
{
	return TypeTag.IsValid();
}

FKRAppliedEquipmentEntry& FKREquipmentList::FindEntryByItemTag(FGameplayTag InTypeTag)
{
	for (FKRAppliedEquipmentEntry& Entry : Entries)
	{
		if (Entry.TypeTag == InTypeTag)
		{
			return Entry;
		}
	}

	UE_LOG(LogEquipmentManagerComponent, Error, TEXT("Invalid ItemTag: %s"), *InTypeTag.ToString());

	return Invalid_Entry;
}

const FKRAppliedEquipmentEntry& FKREquipmentList::FindEntryByItemTag(FGameplayTag InTypeTag) const
{
	for (const FKRAppliedEquipmentEntry& Entry : Entries)
	{
		if (Entry.TypeTag == InTypeTag)
		{
			return Entry;
		}
	}

	UE_LOG(LogEquipmentManagerComponent, Error, TEXT("Invalid ItemTag: %s"), *InTypeTag.ToString());

	return Invalid_Entry;
}

UKREquipmentManagerComponent::UKREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void UKREquipmentManagerComponent::EquipItem(UKRInventoryItemInstance* ItemInstance)
{
	if (ItemInstance == nullptr)
	{
		return;
	}

	const UInventoryFragment_EquippableItem* EquippableFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_EquippableItem>();
	if (EquippableFragment == nullptr)
	{
		UE_LOG(LogEquipmentManagerComponent, Error, TEXT("This Item not equippable"));
		
		return;
	}
	
	const UInventoryFragment_SetStats* SetStatFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_SetStats>();
	
	const FGameplayTag& ItemTag = ItemInstance->GetItemTag();
	const FKRAppliedEquipmentEntry& Entry = FindEntryByTag(ItemTag);
	if (!Entry.IsValid())
	{
		return;
	}

	Entry.ItemInstance = ItemInstance;
	ACharacter* Character = CastChecked<ACharacter>(GetOwner());
	APlayerController* PC = CastChecked<APlayerController>(Character->GetController());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	// Init Stats
	if (SetStatFragment)
	{
		ApplyStatsToASC(SetStatFragment, true);
	}
	
	// GrantAbilities
	if (ASC)
	{
		for (const FKRAbilitySet_GameplayAbility& AbilityInfo : Entry.GrantedAbilities)
		{
			if (!AbilityInfo.Ability)
			{
				continue;
			}

			FGameplayAbilitySpec AbilitySpec(AbilityInfo.Ability, AbilityInfo.AbilityLevel, -1, this);

			if (AbilityInfo.InputTag.IsValid())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityInfo.InputTag);
			}

			FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(AbilitySpec);
			Entry.GrantedHandles.AddAbilitySpecHandle(Handle);
		}
	}

	// Apply Anim Layer
	if (Entry.EquipAnimLayer)
	{
		Character->GetMesh()->LinkAnimClassLayers(Entry.EquipAnimLayer);
	}

	// Add Input Context
	if (PC && Entry.EquipIMC)
	{
		if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(Entry.EquipIMC);
				Subsystem->AddMappingContext(Entry.EquipIMC, Entry.InputPriority);
			}
		}
	}

	AKRWeaponBase* TargetWeapon = nullptr;
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORD))
	{
		TargetWeapon = MeleeActorInstance;
	}
	else if(ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN))
	{
		TargetWeapon = RangeActorInstance;
	}

	if (TargetWeapon)
	{
		if (Entry.SocketName != NAME_None)
		{
			TargetWeapon->AttachToComponent(
				Character->GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				Entry.SocketName
				);
			TargetWeapon->SetActorRelativeTransform(Entry.AttachTransform);
		}

		TargetWeapon->ConfigureWeapon(EquippableFragment, SetStatFragment);
		TargetWeapon->SetWeaponVisibility(true);
		TargetWeapon->PlayEquipEffect();
	}

	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_COSTUME))
	{
		if (USkeletalMeshComponent* SKMeshComp = Character->GetMesh())
		{
			static FName CostumeSlotName1 = FName("MAT_CB_DRESS1");
			static FName CostumeSlotName2 = FName("MAT_CB_SHOES");
			static FName CostumeSlotName3 = FName("MAT_CB_ARM");
				
			int32 CostumeIndex1 = SKMeshComp->GetMaterialIndex(CostumeSlotName1);
			int32 CostumeIndex2 = SKMeshComp->GetMaterialIndex(CostumeSlotName2);
			int32 CostumeIndex3 = SKMeshComp->GetMaterialIndex(CostumeSlotName3);

			if (UKREquipmentInstance* Instance = EquippableFragment->GetEquipInstance())
			{
				if (const UKREquipmentDefinition* Definition = Instance->GetDefinition())
				{
					if (const FEquipDataStruct* EquipDataStruct = Definition->GetEquipDataStruct())
					{
						SKMeshComp->SetMaterial(CostumeIndex1, EquipDataStruct->OverrideMaterials[0].LoadSynchronous());
						SKMeshComp->SetMaterial(CostumeIndex2, EquipDataStruct->OverrideMaterials[1].LoadSynchronous());
						SKMeshComp->SetMaterial(CostumeIndex3, EquipDataStruct->OverrideMaterials[2].LoadSynchronous());
					}
				}
			}
		}
	}

	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_HEAD))
	{
		if (USkeletalMeshComponent* SKMeshComp = Character->GetMesh())
		{
			static FName HeadSlotName = FName("MAT_CB_EARS_SOC");
				
			int32 HeadIndex = SKMeshComp->GetMaterialIndex(HeadSlotName);

			if (UKREquipmentInstance* Instance = EquippableFragment->GetEquipInstance())
			{
				if (const UKREquipmentDefinition* Definition = Instance->GetDefinition())
				{
					if (const FEquipDataStruct* EquipDataStruct = Definition->GetEquipDataStruct())
					{
						SKMeshComp->SetMaterial(HeadIndex, EquipDataStruct->OverrideMaterials[0].LoadSynchronous());
					}
				}
			}
		}
	}

	//BroadcastWeaponMessage();
}

void UKREquipmentManagerComponent::UnequipItem(UKRInventoryItemInstance* ItemInstance)
{
	if (ItemInstance == nullptr)
	{
		return;
	}

	const FGameplayTag& ItemTag = ItemInstance->GetItemTag();
	const FKRAppliedEquipmentEntry& Entry = FindEntryByTag(ItemTag);
	if (!Entry.IsValid())
	{
		return;
	}

	// Remove Ability
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
	}

	// Remove Stat
	const UInventoryFragment_SetStats* SetStatFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_SetStats>();
	if (SetStatFragment)
	{
		ApplyStatsToASC(SetStatFragment, false);
	}

	// Remove Anim Layer
	ACharacter* Character = CastChecked<ACharacter>(GetOwner());
	if (Character && Entry.EquipAnimLayer)
	{
		Character->GetMesh()->UnlinkAnimClassLayers(Entry.EquipAnimLayer);
	}

	// Remove Input Context
	APlayerController* PC = CastChecked<APlayerController>(Character->GetController());
	if (PC && Entry.EquipIMC)
	{
		if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(Entry.EquipIMC);
			}
		}
	}

	AKRWeaponBase* TargetWeapon = nullptr;
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORD))
	{
		TargetWeapon = MeleeActorInstance;
	}
	else if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN))
	{
		TargetWeapon = RangeActorInstance;
	}

	if (TargetWeapon)
	{
		TargetWeapon->PlayUnequipEffect();
		TargetWeapon->SetWeaponVisibility(false);
	}

	Entry.ItemInstance = nullptr;
}

void UKREquipmentManagerComponent::ChangeEquipment(class UKRInventoryItemInstance* OldItem,
	class UKRInventoryItemInstance* NewItem)
{
	if (OldItem)
	{
		UnequipItem(OldItem);
	}
	if (NewItem)
	{
		EquipItem(NewItem);
	}
}

const FKRAppliedEquipmentEntry& UKREquipmentManagerComponent::FindEntryByTag(const FGameplayTag& ItemTag) const
{
	for (const auto& EntryListPair : EquipSlotTagToEquipmentListMap)
	{
		if (EntryListPair.Key.MatchesTag(ItemTag))
		{
			const FKREquipmentList& EquipmentList = EntryListPair.Value;
			const FKRAppliedEquipmentEntry& Entry = EquipmentList.FindEntryByItemTag(ItemTag);

			return Entry;
		}
	}

	return Invalid_Entry;
}

UKRInventoryItemInstance* UKREquipmentManagerComponent::GetEquippedItemInstanceBySlotTag(
	const FGameplayTag& SlotTag) const
{
	const FKREquipmentList* EquipmentList = EquipSlotTagToEquipmentListMap.Find(SlotTag);
	if (!EquipmentList)
	{
		return nullptr;
	}
	
	for (const FKRAppliedEquipmentEntry& Entry : EquipmentList->Entries)
	{
		if (Entry.IsValid() && Entry.ItemInstance)
		{
			return Entry.ItemInstance;
		}
	}

	return nullptr;
}

void UKREquipmentManagerComponent::ApplyStatsToASC(const class UInventoryFragment_SetStats* SetStatsFragment, bool bAdd)
{
	if (!SetStatsFragment) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;
	
}

void UKREquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//AddEquip();
}

void UKREquipmentManagerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

void UKREquipmentManagerComponent::AddEquip(UKRInventoryItemInstance* ItemInstance)
{
	for (auto& EquipSlot : EquipSlotTagToEquipmentListMap)
	{
		if (EquipSlot.Key.MatchesTag(ItemInstance->GetItemTag()))
		{
			if (UKRInventoryItemDefinition* ItemDefinition = ItemInstance->GetItemDef())
			{
				if (UInventoryFragment_EquippableItem* EquippableFragment
					= Cast<UInventoryFragment_EquippableItem>(ItemDefinition->FindFragmentByTag(KRTAG_FRAGMENT_ITEM_EQUIPPABLE)))
				{
					UKREquipmentInstance* NewEquipInstance = NewObject<UKREquipmentInstance>(this);
					const FEquipDataStruct* EquipDataStruct = UKRDataTablesSubsystem::Get(this).GetData<FEquipDataStruct>(EGameDataType::EquipData, ItemInstance->GetItemTag());

					NewEquipInstance->InitializeFromData(EquipDataStruct);

					EquippableFragment->SetEquipInstance(NewEquipInstance);
				}
			}

			break;
		}
	}
}

void UKREquipmentManagerComponent::SpawnActorInstances()
{
	ACharacter* Character = CastChecked<ACharacter>(GetOwner());
	
	if (MeleeActorClass)
	{
		AKRMeleeWeapon* MeleeActor = GetWorld()->SpawnActorDeferred<AKRMeleeWeapon>(MeleeActorClass, FTransform::Identity, GetOwner(), Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (MeleeActor)
		{
			MeleeActorInstance = MeleeActor;
			MeleeActor->FinishSpawning(FTransform::Identity, true);
			MeleeActor->SetActorHiddenInGame(true);
		}
	}

	if (RangeActorClass)
	{
		AKRRangeWeapon* RangeActor = GetWorld()->SpawnActorDeferred<AKRRangeWeapon>(RangeActorClass, FTransform::Identity, GetOwner(), Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (RangeActor)
		{
			RangeActorInstance = RangeActor;
			RangeActor->FinishSpawning(FTransform::Identity, true);
			RangeActor->SetActorHiddenInGame(true);
		}
	}
}

