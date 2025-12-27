#include "KREquipmentManagerComponent.h"
#include "KREquipmentInstance.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/KRPawnExtensionComponent.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "Inventory/Fragment/InventoryFragment_ModuleItem.h"
#include "SubSystem/KRInventorySubsystem.h"
#include "GameFramework/Character.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Data/EquipAbilityDataStruct.h"
#include "GameplayEffect.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "Engine/AssetManager.h"
#include "Data/EquipDataStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentManagerComponent)

DEFINE_LOG_CATEGORY(LogEquipmentManagerComponent);

static FGameplayTag GetSlotTagFromItemTag(const FGameplayTag& ItemTag);

static FKRAppliedEquipmentEntry& GetInvalidEntry() // To prevent ODR
{
	static FKRAppliedEquipmentEntry Invalid_Entry;
	return Invalid_Entry;
}

bool FKRAppliedEquipmentEntry::IsValid() const
{
	return TypeTag.IsValid();
}

FKRAppliedEquipmentEntry& FKREquipmentList::FindEntryByTagMatch(FGameplayTag InItemTag)
{
	for (FKRAppliedEquipmentEntry& Entry : Entries)
	{
		if (InItemTag.MatchesTag(Entry.TypeTag))
		{
			return Entry;
		}
	}

	return GetInvalidEntry();
}

const FKRAppliedEquipmentEntry& FKREquipmentList::FindEntryByTagMatch(FGameplayTag InItemTag) const
{
	for (const FKRAppliedEquipmentEntry& Entry : Entries)
	{
		if (InItemTag.MatchesTag(Entry.TypeTag))
		{
			return Entry;
		}
	}

	return GetInvalidEntry();
}

FGameplayTag GetSlotTagFromItemTag(const FGameplayTag& ItemTag)
{
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORD)) return KRTAG_ITEMTYPE_EQUIP_SWORD;
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN)) return KRTAG_ITEMTYPE_EQUIP_GUN;
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_HEAD)) return KRTAG_ITEMTYPE_EQUIP_HEAD;
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_COSTUME)) return KRTAG_ITEMTYPE_EQUIP_COSTUME;
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORDMODULE)) return KRTAG_ITEMTYPE_EQUIP_SWORDMODULE;
	if (ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUNMODULE)) return KRTAG_ITEMTYPE_EQUIP_GUNMODULE;
	return FGameplayTag();
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

void UKREquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(GetOwner()))
	{
		PawnExtComp->OnAbilitySystemInitialized.AddUObject(this, &UKREquipmentManagerComponent::OnAbilitySystemInitialized);

		// 이미 ASC가 초기화되었다면 직접 호출
		if (PawnExtComp->GetKRAbilitySystemComponent())
		{
			OnAbilitySystemInitialized();
		}
	}

	ConfirmMessageHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		FKRUIMessageTags::Confirm(),
		this,
		&UKREquipmentManagerComponent::OnEquipmentConfirmMessage);
}

void UKREquipmentManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ConfirmMessageHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(ConfirmMessageHandle);
	}
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PrewarmTimerHandle);
		World->GetTimerManager().ClearTimer(EquipBroadcastDelayHandle);
	}

	if (MaterialPreloadHandle.IsValid())
	{
		MaterialPreloadHandle->CancelHandle();
		MaterialPreloadHandle.Reset();
	}
	CachedMaterials.Empty();

	Super::EndPlay(EndPlayReason);
}

void UKREquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UKREquipmentManagerComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

void UKREquipmentManagerComponent::AddEquip(UKRInventoryItemInstance* ItemInstance)
{
	if (!ItemInstance) return;

	const FGameplayTag& ItemTag = ItemInstance->GetItemTag();
	const FGameplayTag SlotTag = GetSlotTagFromItemTag(ItemTag);
	if (!SlotTag.IsValid()) return;
	
	if (IsWeaponSlot(SlotTag) || SlotTag == KRTAG_ITEMTYPE_EQUIP_HEAD || SlotTag == KRTAG_ITEMTYPE_EQUIP_COSTUME)
	{
		UInventoryFragment_EquippableItem* EquippableFragment = ItemInstance->FindInstanceFragmentByClass<UInventoryFragment_EquippableItem>();
		if (!EquippableFragment)
		{
			UE_LOG(LogEquipmentManagerComponent, Warning, TEXT("AddEquip: EquippableFragment not found for [%s]"), *ItemTag.ToString());
			return;
		}
		
		if (!EquippableFragment->IsInitialized())
		{
			EquippableFragment->OnInstanceCreated(ItemInstance);
		}
		
		if (!EquippableFragment->IsInitialized())
		{
			UE_LOG(LogEquipmentManagerComponent, Warning, TEXT("AddEquip: Failed to initialize EquippableFragment for [%s]"), *ItemTag.ToString());
			return;
		}
	}
	else if (IsModuleSlot(SlotTag))
	{
		UInventoryFragment_ModuleItem* ModuleFragment = ItemInstance->FindInstanceFragmentByClass<UInventoryFragment_ModuleItem>();

		if (!ModuleFragment)
		{
			ModuleFragment = NewObject<UInventoryFragment_ModuleItem>(ItemInstance);
			ItemInstance->AddInstanceFragment(KRTAG_FRAGMENT_ITEM_MODULE, ModuleFragment);
			ModuleFragment->OnInstanceCreated(ItemInstance);
		}
		else if (!ModuleFragment->IsInitialized())
		{
			ModuleFragment->OnInstanceCreated(ItemInstance);
		}
	}
}

void UKREquipmentManagerComponent::SpawnActorInstances()
{
	if (MeleeActorInstance || RangeActorInstance)
	{
		return;
	}

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
	AttachWeaponsToCharacter();
	PrewarmWeaponEffects();
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
		UE_LOG(LogEquipmentManagerComponent, Error, TEXT("This Item [%s] not equippable - EquippableFragment not found"), *ItemInstance->GetItemTag().ToString());
		return;
	}

	const FGameplayTag& ItemTag = ItemInstance->GetItemTag();
	const FGameplayTag SlotTag = GetSlotTagFromItemTag(ItemTag);
	if (!SlotTag.IsValid())
	{
		UE_LOG(LogEquipmentManagerComponent, Error, TEXT("EquipItem: Invalid SlotTag for ItemTag [%s]"), *ItemTag.ToString());
		return;
	}

	ACharacter* Character = CastChecked<ACharacter>(GetOwner());
	if (IsWeaponSlot(SlotTag))
	{
		const FKRAppliedEquipmentEntry& Entry = FindEntryByTag(ItemTag);
		if (!Entry.IsValid())
		{
			UE_LOG(LogEquipmentManagerComponent, Error, TEXT("EquipItem: No valid Entry for weapon [%s]"), *ItemTag.ToString());
			return;
		}
		EquippedItemMap.Add(SlotTag, ItemInstance);

		const UInventoryFragment_SetStats* SetStatFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_SetStats>();
		if (SetStatFragment && SetStatFragment->IsInitialized())
		{
			const FWeaponStatsCache& Stats = SetStatFragment->GetWeaponStats();
			ApplyEquipmentStats(Stats, SlotTag);
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
			TargetWeapon->ConfigureWeapon(ItemInstance);
		}

		ScheduleEquipSlotBroadcast(SlotTag, ItemInstance);
	}
	else if (SlotTag == KRTAG_ITEMTYPE_EQUIP_COSTUME)
	{
		EquippedItemMap.Add(SlotTag, ItemInstance);

		if (USkeletalMeshComponent* SKMeshComp = Character->GetMesh())
		{
			static FName CostumeSlotName1 = FName("MAT_CB_ARM");
			static FName CostumeSlotName2 = FName("MAT_CB_DRESS1");
			static FName CostumeSlotName3 = FName("MAT_CB_SHOES");

			int32 CostumeIndex1 = SKMeshComp->GetMaterialIndex(CostumeSlotName1);
			int32 CostumeIndex2 = SKMeshComp->GetMaterialIndex(CostumeSlotName2);
			int32 CostumeIndex3 = SKMeshComp->GetMaterialIndex(CostumeSlotName3);

			if (const UKREquipmentInstance* Instance = EquippableFragment->GetEquipInstance())
			{
				if (Instance->IsValid())
				{
					const FEquipDataStruct& EquipData = Instance->GetEquipData();
					if (EquipData.OverrideMaterials.Num() >= 3)
					{
						if (UMaterialInterface* Mat0 = GetCachedMaterial(EquipData.OverrideMaterials[0]))
						{
							SKMeshComp->SetMaterial(CostumeIndex1, Mat0);
						}
						else if (UMaterialInterface* Mat0Sync = EquipData.OverrideMaterials[0].LoadSynchronous())
						{
							SKMeshComp->SetMaterial(CostumeIndex1, Mat0Sync);
						}

						if (UMaterialInterface* Mat1 = GetCachedMaterial(EquipData.OverrideMaterials[1]))
						{
							SKMeshComp->SetMaterial(CostumeIndex2, Mat1);
						}
						else if (UMaterialInterface* Mat1Sync = EquipData.OverrideMaterials[1].LoadSynchronous())
						{
							SKMeshComp->SetMaterial(CostumeIndex2, Mat1Sync);
						}

						if (UMaterialInterface* Mat2 = GetCachedMaterial(EquipData.OverrideMaterials[2]))
						{
							SKMeshComp->SetMaterial(CostumeIndex3, Mat2);
						}
						else if (UMaterialInterface* Mat2Sync = EquipData.OverrideMaterials[2].LoadSynchronous())
						{
							SKMeshComp->SetMaterial(CostumeIndex3, Mat2Sync);
						}
					}
				}
			}
		}
		ScheduleEquipSlotBroadcast(SlotTag, ItemInstance);
	}
	else if (SlotTag == KRTAG_ITEMTYPE_EQUIP_HEAD)
	{
		EquippedItemMap.Add(SlotTag, ItemInstance);

		if (USkeletalMeshComponent* SKMeshComp = Character->GetMesh())
		{
			static FName HeadSlotName = FName("MAT_CB_EARS_SOCK");
			int32 HeadIndex = SKMeshComp->GetMaterialIndex(HeadSlotName);

			if (const UKREquipmentInstance* Instance = EquippableFragment->GetEquipInstance())
			{
				if (Instance->IsValid())
				{
					const FEquipDataStruct& EquipData = Instance->GetEquipData();
					if (EquipData.OverrideMaterials.Num() >= 1)
					{
						if (UMaterialInterface* Mat = GetCachedMaterial(EquipData.OverrideMaterials[0]))
						{
							SKMeshComp->SetMaterial(HeadIndex, Mat);
						}
						else if (UMaterialInterface* MatSync = EquipData.OverrideMaterials[0].LoadSynchronous())
						{
							SKMeshComp->SetMaterial(HeadIndex, MatSync);
						}
					}
				}
			}
		}
		ScheduleEquipSlotBroadcast(SlotTag, ItemInstance);
	}
	else if (IsModuleSlot(SlotTag))
	{
		EquippedItemMap.Add(SlotTag, ItemInstance);
		ApplyModuleStats(ItemInstance);
		ScheduleEquipSlotBroadcast(SlotTag, ItemInstance);
	}
}

void UKREquipmentManagerComponent::UnequipItem(UKRInventoryItemInstance* ItemInstance)
{
	if (ItemInstance == nullptr)
	{
		return;
	}

	const FGameplayTag& ItemTag = ItemInstance->GetItemTag();
	const FGameplayTag SlotTag = GetSlotTagFromItemTag(ItemTag);

	if (IsWeaponSlot(SlotTag))
	{
		const FKRAppliedEquipmentEntry& Entry = FindEntryByTag(ItemTag);
		if (!Entry.IsValid())
		{
			return;
		}
		
		RemoveEquipmentStats(SlotTag);
		if (bCombatModeActive && ActiveWeaponTypeTag.MatchesTag(ItemTag))
		{
			DeactivateCombatMode();
		}
		EquippedItemMap.Remove(SlotTag);
	}

	else if (SlotTag == KRTAG_ITEMTYPE_EQUIP_HEAD || SlotTag == KRTAG_ITEMTYPE_EQUIP_COSTUME)
	{
		EquippedItemMap.Remove(SlotTag);
	}

	else if (IsModuleSlot(SlotTag))
	{
		RemoveModuleStats(ItemInstance);
		EquippedItemMap.Remove(SlotTag);
	}
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
	for (const auto& EntryListPair : WeaponEquipmentMap)
	{
		if (ItemTag.MatchesTag(EntryListPair.Key))
		{
			const FKREquipmentList& EquipmentList = EntryListPair.Value;
			const FKRAppliedEquipmentEntry& Entry = EquipmentList.FindEntryByTagMatch(ItemTag);

			return Entry;
		}
	}

	return GetInvalidEntry();
}

UKRInventoryItemInstance* UKREquipmentManagerComponent::GetEquippedItemInstanceBySlotTag(
	const FGameplayTag& SlotTag) const
{
	if (const TObjectPtr<UKRInventoryItemInstance>* Found = EquippedItemMap.Find(SlotTag))
	{
		return *Found;
	}

	return nullptr;
}

void UKREquipmentManagerComponent::AttachWeaponsToCharacter()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (!MeshComp)
	{
		return;
	}

	// Melee 무기 Attach (숨김 상태)
	if (MeleeActorInstance)
	{
		const FKRAppliedEquipmentEntry& MeleeEntry = FindEntryByTag(KRTAG_ITEMTYPE_EQUIP_SWORD);
		if (MeleeEntry.IsValid() && MeleeEntry.SocketName != NAME_None)
		{
			MeleeActorInstance->AttachToComponent(
				MeshComp,
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				MeleeEntry.SocketName
			);
			MeleeActorInstance->SetActorRelativeTransform(MeleeEntry.AttachTransform);

			if (UKRInventoryItemInstance* MeleeItemInstance = GetEquippedItemInstanceBySlotTag(KRTAG_ITEMTYPE_EQUIP_SWORD))
			{
				MeleeActorInstance->ConfigureWeapon(MeleeItemInstance);
			}
		}
		MeleeActorInstance->SetWeaponVisibility(false);
		UE_LOG(LogEquipmentManagerComponent, Log, TEXT("AttachWeaponsToCharacter: Melee weapon attached (hidden)"));
	}

	// Range 무기 Attach (숨김 상태)
	if (RangeActorInstance)
	{
		const FKRAppliedEquipmentEntry& RangeEntry = FindEntryByTag(KRTAG_ITEMTYPE_EQUIP_GUN);
		if (RangeEntry.IsValid() && RangeEntry.SocketName != NAME_None)
		{
			RangeActorInstance->AttachToComponent(
				MeshComp,
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				RangeEntry.SocketName
			);
			RangeActorInstance->SetActorRelativeTransform(RangeEntry.AttachTransform);

			if (UKRInventoryItemInstance* RangeItemInstance = GetEquippedItemInstanceBySlotTag(KRTAG_ITEMTYPE_EQUIP_GUN))
			{
				RangeActorInstance->ConfigureWeapon(RangeItemInstance);
			}
		}
		RangeActorInstance->SetWeaponVisibility(false);
		UE_LOG(LogEquipmentManagerComponent, Log, TEXT("AttachWeaponsToCharacter: Range weapon attached (hidden)"));
	}
}

bool UKREquipmentManagerComponent::ActivateCombatMode(const FGameplayTag& WeaponTypeTag)
{
	// 이미 같은 무기 타입이 활성화되어 있으면 무시
	if (bCombatModeActive && ActiveWeaponTypeTag == WeaponTypeTag)
	{
		return true;
	}

	// 다른 무기가 활성화되어 있으면 먼저 비활성화
	if (bCombatModeActive)
	{
		DeactivateCombatMode();
	}

	const FKRAppliedEquipmentEntry& Entry = FindEntryByTag(WeaponTypeTag);
	if (!Entry.IsValid())
	{
		UE_LOG(LogEquipmentManagerComponent, Warning, TEXT("ActivateCombatMode: Invalid WeaponTypeTag [%s]"), *WeaponTypeTag.ToString());
		return false;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return false;
	}

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	// 1. GA 부여
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
			CombatModeGrantedHandles.AddAbilitySpecHandle(Handle);
		}
	}

	// 2. AnimLayer 적용
	if (Entry.EquipAnimLayer)
	{
		Character->GetMesh()->LinkAnimClassLayers(Entry.EquipAnimLayer);
		ActiveAnimLayer = Entry.EquipAnimLayer;
	}

	// 3. IMC 추가
	if (PC && Entry.EquipIMC)
	{
		if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(Entry.EquipIMC);
				Subsystem->AddMappingContext(Entry.EquipIMC, Entry.InputPriority);
				ActiveIMC = Entry.EquipIMC;
			}
		}
	}

	// 4. 무기 가시성 활성화 (이펙트 포함)
	AKRWeaponBase* TargetWeapon = nullptr;
	AKRWeaponBase* OtherWeapon = nullptr; // 꺼야 할 무기

	UE_LOG(LogTemp, Error, TEXT("WeaponTypeTag : %s"), *WeaponTypeTag.ToString());
	
	if (WeaponTypeTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORD))
	{
		TargetWeapon = MeleeActorInstance;
		OtherWeapon = RangeActorInstance;
	}
	else if (WeaponTypeTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN))
	{
		TargetWeapon = RangeActorInstance;
		OtherWeapon = MeleeActorInstance;
	}
	
	// 활성화 대상: 이펙트 재생 (내부에서 SetVisible(true) 호출됨)
	if (TargetWeapon)
	{
		TargetWeapon->PlayEquipEffect(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TargetWeapon is nullptr"));
	}

	// 비활성화 대상: 이펙트 재생 (내부에서 SetVisible(false) 호출됨)
	if (OtherWeapon)
	{
		OtherWeapon->PlayUnequipEffect();
	}

	// 상태 업데이트
	bCombatModeActive = true;
	ActiveWeaponTypeTag = WeaponTypeTag;

	BroadcastWeaponMessage(EWeaponMessageAction::Switched, WeaponTypeTag);

	UE_LOG(LogEquipmentManagerComponent, Log, TEXT("ActivateCombatMode: Activated for WeaponTypeTag [%s]"), *WeaponTypeTag.ToString());
	return true;
}

void UKREquipmentManagerComponent::DeactivateCombatMode()
{
	if (!bCombatModeActive)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	// 1. GA 제거
	if (ASC)
	{
		CombatModeGrantedHandles.TakeFromAbilitySystem(ASC);
	}

	// 2. AnimLayer 제거
	if (ActiveAnimLayer)
	{
		Character->GetMesh()->UnlinkAnimClassLayers(ActiveAnimLayer);
		ActiveAnimLayer = nullptr;
	}

	// 3. IMC 제거
	if (PC && ActiveIMC)
	{
		if (const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->RemoveMappingContext(ActiveIMC);
			}
		}
		ActiveIMC = nullptr;
	}

	// 4. 모든 무기 숨기기
	if (MeleeActorInstance)
	{
		MeleeActorInstance->PlayUnequipEffect();
	}
	if (RangeActorInstance)
	{
		RangeActorInstance->PlayUnequipEffect();
	}

	// 상태 업데이트
	bCombatModeActive = false;
	ActiveWeaponTypeTag = FGameplayTag();
	BroadcastWeaponMessage(EWeaponMessageAction::Unequipped);

	UE_LOG(LogEquipmentManagerComponent, Log, TEXT("DeactivateCombatMode: Combat mode deactivated"));
}

FActiveGameplayEffectHandle UKREquipmentManagerComponent::ApplyEquipmentStats(const FWeaponStatsCache& Stats, const FGameplayTag& SlotTag)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !Stats.HasAnyModifier())
	{
		return FActiveGameplayEffectHandle();
	}
	
	RemoveEquipmentStats(SlotTag);
	
	UGameplayEffect* GE = nullptr;
	if (TObjectPtr<UGameplayEffect>* CachedGE = EquipmentGEObjects.Find(SlotTag))
	{
		GE = *CachedGE;
		if (GE)
		{
			GE->Modifiers.Empty();
		}
	}

	if (!GE)
	{
		FString GEName = FString::Printf(TEXT("GE_Equip_%s"), *SlotTag.ToString().Replace(TEXT("."), TEXT("_")));
		GE = NewObject<UGameplayEffect>(this, FName(*GEName));
		GE->DurationPolicy = EGameplayEffectDurationType::Infinite;
		EquipmentGEObjects.Add(SlotTag, GE); // GC
	}

	int32 Idx = 0;
	const float AttackPowerDelta = Stats.AttackPower - FWeaponStatsCache::DefaultAttackPower;
	if (!FMath::IsNearlyZero(AttackPowerDelta))
	{
		GE->Modifiers.Add(FGameplayModifierInfo());
		GE->Modifiers[Idx].Attribute = UKRCombatCommonSet::GetAttackPowerAttribute();
		GE->Modifiers[Idx].ModifierOp = EGameplayModOp::Additive;
		GE->Modifiers[Idx].ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(AttackPowerDelta));
		Idx++;
	}

	const float AttackSpeedDelta = Stats.AttackSpeed - FWeaponStatsCache::DefaultAttackSpeed;
	if (!FMath::IsNearlyZero(AttackSpeedDelta))
	{
		GE->Modifiers.Add(FGameplayModifierInfo());
		GE->Modifiers[Idx].Attribute = UKRCombatCommonSet::GetAttackSpeedAttribute();
		GE->Modifiers[Idx].ModifierOp = EGameplayModOp::Additive;
		GE->Modifiers[Idx].ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(AttackSpeedDelta));
		Idx++;
	}

	const float CritChanceDelta = Stats.CritChance - FWeaponStatsCache::DefaultCritChance;
	if (!FMath::IsNearlyZero(CritChanceDelta))
	{
		GE->Modifiers.Add(FGameplayModifierInfo());
		GE->Modifiers[Idx].Attribute = UKRCombatCommonSet::GetCritChanceAttribute();
		GE->Modifiers[Idx].ModifierOp = EGameplayModOp::Additive;
		GE->Modifiers[Idx].ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(CritChanceDelta));
		Idx++;
	}

	const float CritMultiDelta = Stats.CritMulti - FWeaponStatsCache::DefaultCritMulti;
	if (!FMath::IsNearlyZero(CritMultiDelta))
	{
		GE->Modifiers.Add(FGameplayModifierInfo());
		GE->Modifiers[Idx].Attribute = UKRCombatCommonSet::GetCritMultiAttribute();
		GE->Modifiers[Idx].ModifierOp = EGameplayModOp::Additive;
		GE->Modifiers[Idx].ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(CritMultiDelta));
		Idx++;
	}

	const float RangeDelta = Stats.Range - FWeaponStatsCache::DefaultRange;
	if (!FMath::IsNearlyZero(RangeDelta))
	{
		GE->Modifiers.Add(FGameplayModifierInfo());
		GE->Modifiers[Idx].Attribute = UKRCombatCommonSet::GetWeaponRangeAttribute();
		GE->Modifiers[Idx].ModifierOp = EGameplayModOp::Additive;
		GE->Modifiers[Idx].ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(RangeDelta));
		Idx++;
	}
	
	if (Idx == 0)
	{
		return FActiveGameplayEffectHandle();
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);
	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectToSelf(GE, 1.0f, Context);
	
	if (Handle.IsValid())
	{
		EquipmentGEHandles.Add(SlotTag, Handle);
	}

	UE_LOG(LogEquipmentManagerComponent, Log, TEXT("ApplyEquipmentStats [%s]: Applied GE with ATK=%.1f, Speed=%.2f, Crit=%.2f"),
		*SlotTag.ToString(), Stats.AttackPower, Stats.AttackSpeed, Stats.CritChance);

	return Handle;
}

void UKREquipmentManagerComponent::RemoveEquipmentStats(const FGameplayTag& SlotTag)
{
	FActiveGameplayEffectHandle* HandlePtr = EquipmentGEHandles.Find(SlotTag);
	if (HandlePtr && HandlePtr->IsValid())
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->RemoveActiveGameplayEffect(*HandlePtr);
		}
		EquipmentGEHandles.Remove(SlotTag);
	}
}

void UKREquipmentManagerComponent::ApplyModuleStats(UKRInventoryItemInstance* ModuleInstance)
{
	if (!ModuleInstance)
	{
		return;
	}

	// 모듈은 ItemTag를 키로 사용 (SlotTag 사용 시 동일 슬롯 모듈끼리 GE가 덮어씌워짐)
	const FGameplayTag& ItemTag = ModuleInstance->GetItemTag();
	if (!ItemTag.IsValid())
	{
		return;
	}

	const UInventoryFragment_ModuleItem* ModuleFragment = ModuleInstance->FindFragmentByClass<UInventoryFragment_ModuleItem>();
	if (!ModuleFragment || !ModuleFragment->IsInitialized())
	{
		return;
	}

	const TArray<FKREquipAbilityModifierRow>& Modifiers = ModuleFragment->GetStatModifiers();
	FWeaponStatsCache Stats = ConvertModifiersToStatsCache(Modifiers);

	ApplyEquipmentStats(Stats, ItemTag);
}

void UKREquipmentManagerComponent::RemoveModuleStats(UKRInventoryItemInstance* ModuleInstance)
{
	if (!ModuleInstance)
	{
		return;
	}
	const FGameplayTag& ItemTag = ModuleInstance->GetItemTag();
	if (!ItemTag.IsValid())
	{
		return;
	}

	RemoveEquipmentStats(ItemTag);
}

FWeaponStatsCache UKREquipmentManagerComponent::ConvertModifiersToStatsCache(const TArray<FKREquipAbilityModifierRow>& Modifiers)
{
	FWeaponStatsCache Stats;

	for (const FKREquipAbilityModifierRow& Modifier : Modifiers)
	{
		if (!Modifier.AbilityTypeTag.IsValid())
		{
			continue;
		}

		const float Value = Modifier.GetFinalValue();
		auto ApplyModifier = [&Modifier, Value](float& StatValue)
		{
			switch (Modifier.Op)
			{
			case EModifierOp::Absolute:
				StatValue = Value;
				break;
			case EModifierOp::AdditiveDelta:
				StatValue += Value;
				break;
			case EModifierOp::Multiplier:
				StatValue *= Value;
				break;
			}
		};

		if (Modifier.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_ATK))
		{
			ApplyModifier(Stats.AttackPower);
		}
		else if (Modifier.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_ATTACKSPEED))
		{
			ApplyModifier(Stats.AttackSpeed);
		}
		else if (Modifier.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_CRITCHANCE))
		{
			ApplyModifier(Stats.CritChance);
		}
		else if (Modifier.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_CRITMULTI))
		{
			ApplyModifier(Stats.CritMulti);
		}
		else if (Modifier.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_RANGE))
		{
			ApplyModifier(Stats.Range);
		}
	}

	Stats.MarkInitialized();
	return Stats;
}

bool UKREquipmentManagerComponent::IsWeaponSlot(const FGameplayTag& SlotTag) const
{
	return SlotTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORD) || SlotTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN);
}

bool UKREquipmentManagerComponent::IsModuleSlot(const FGameplayTag& SlotTag) const
{
	return SlotTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORDMODULE) || SlotTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUNMODULE);
}

void UKREquipmentManagerComponent::OnAbilitySystemInitialized()
{
	if (bASCInitCallbackProcessed)
	{
		return;
	}
	bASCInitCallbackProcessed = true;

	SpawnActorInstances();
}

void UKREquipmentManagerComponent::OnEquipmentConfirmMessage(FGameplayTag Channel, const FKRUIMessage_Confirm& Payload)
{
	if (Payload.Context != EConfirmContext::Equipment || Payload.Result != EConfirmResult::Yes) return;
	if (!Payload.ItemTag.IsValid())
	{
		UE_LOG(LogEquipmentManagerComponent, Warning, TEXT("OnEquipmentConfirmMessage: Invalid ItemTag"));
		return;
	}

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI) return;
	UKRInventorySubsystem* InvSubsystem = GI->GetSubsystem<UKRInventorySubsystem>();
	if (!InvSubsystem) return;

	TArray<UKRInventoryItemInstance*> FoundItems = InvSubsystem->FindItemsByTag(Payload.ItemTag);
	UKRInventoryItemInstance* NewItemInstance = FoundItems.Num() > 0 ? FoundItems[0] : nullptr;

	if (!NewItemInstance)
	{
		UE_LOG(LogEquipmentManagerComponent, Warning, TEXT("OnEquipmentConfirmMessage: Item [%s] not found in inventory"), *Payload.ItemTag.ToString());
		return;
	}
	
	const FGameplayTag SlotTag = GetSlotTagFromItemTag(Payload.ItemTag);
	UKRInventoryItemInstance* OldItemInstance = GetEquippedItemInstanceBySlotTag(SlotTag);

	AddEquip(NewItemInstance);
	ChangeEquipment(OldItemInstance, NewItemInstance);
}

void UKREquipmentManagerComponent::BroadcastEquipSlotMessage(const FGameplayTag& SlotTag, UKRInventoryItemInstance* ItemInstance)
{
	FKRUIMessage_EquipSlot Msg;
	Msg.SlotTag = SlotTag;
	Msg.ItemInstance = ItemInstance;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::EquipSlot(), Msg);
}

void UKREquipmentManagerComponent::ScheduleEquipSlotBroadcast(const FGameplayTag& SlotTag, UKRInventoryItemInstance* ItemInstance)
{
	PendingBroadcastSlotTag = SlotTag;
	PendingBroadcastItem = ItemInstance;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(EquipBroadcastDelayHandle);
		World->GetTimerManager().SetTimer(
			EquipBroadcastDelayHandle,
			this,
			&UKREquipmentManagerComponent::ExecutePendingBroadcast,
			0.016f,  // 약 1프레임 후 (60fps 기준)
			false
		);
	}
}

void UKREquipmentManagerComponent::ExecutePendingBroadcast()
{
	if (PendingBroadcastSlotTag.IsValid())
	{
		BroadcastEquipSlotMessage(PendingBroadcastSlotTag, PendingBroadcastItem);
		PendingBroadcastSlotTag = FGameplayTag();
		PendingBroadcastItem = nullptr;
	}
}

void UKREquipmentManagerComponent::BroadcastWeaponMessage(EWeaponMessageAction Action, const FGameplayTag& WeaponTypeTag)
{
	FKRUIMessage_Weapon Msg;
	Msg.Action = Action;
	Msg.WeaponTypeTag = WeaponTypeTag;

	// TODO: 총기 탄약 정보 추가
	// if (Action != EWeaponMessageAction::Unequipped && WeaponTypeTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN) && RangeActorInstance)
	// {
	//     Msg.CurrentAmmo = ...;
	//     Msg.MaxAmmo = ...;
	// }

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::Weapon(), Msg);
}

void UKREquipmentManagerComponent::PreloadEquipmentMaterials(const TArray<UKRInventoryItemInstance*>& ItemsToPreload)
{
	if (MaterialPreloadHandle.IsValid() && MaterialPreloadHandle->IsLoadingInProgress())
	{
		return;
	}

	TArray<FSoftObjectPath> AssetsToLoad;

	for (UKRInventoryItemInstance* ItemInstance : ItemsToPreload)
	{
		if (!ItemInstance) continue;

		const UInventoryFragment_EquippableItem* EquippableFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_EquippableItem>();
		if (!EquippableFragment) continue;

		const UKREquipmentInstance* Instance = EquippableFragment->GetEquipInstance();
		if (!Instance || !Instance->IsValid()) continue;

		const FEquipDataStruct& EquipData = Instance->GetEquipData();
		for (const TSoftObjectPtr<UMaterialInterface>& MatRef : EquipData.OverrideMaterials)
		{
			if (!MatRef.IsNull())
			{
				AssetsToLoad.AddUnique(MatRef.ToSoftObjectPath());
			}
		}
	}

	if (AssetsToLoad.Num() == 0)
	{
		return;
	}
	
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	MaterialPreloadHandle = StreamableManager.RequestAsyncLoad(
		AssetsToLoad,
		FStreamableDelegate::CreateUObject(this, &UKREquipmentManagerComponent::OnMaterialsPreloaded),
		FStreamableManager::AsyncLoadHighPriority
	);
}

void UKREquipmentManagerComponent::OnMaterialsPreloaded()
{
	if (!MaterialPreloadHandle.IsValid())
	{
		return;
	}
	
	TArray<UObject*> LoadedAssets;
	MaterialPreloadHandle->GetLoadedAssets(LoadedAssets);

	for (UObject* Asset : LoadedAssets)
	{
		if (UMaterialInterface* Mat = Cast<UMaterialInterface>(Asset))
		{
			FSoftObjectPath Path(Mat);
			CachedMaterials.Add(Path, Mat);
		}
	}
}

UMaterialInterface* UKREquipmentManagerComponent::GetCachedMaterial(const TSoftObjectPtr<UMaterialInterface>& SoftPtr) const
{
	if (SoftPtr.IsNull())
	{
		return nullptr;
	}

	if (SoftPtr.IsValid())
	{
		return SoftPtr.Get();
	}

	if (const TObjectPtr<UMaterialInterface>* Found = CachedMaterials.Find(SoftPtr.ToSoftObjectPath()))
	{
		if (*Found)
		{
			return *Found;
		}
	}

	return nullptr;
}

void UKREquipmentManagerComponent::PrewarmWeaponEffects()
{
	if (bPrewarmCompleted)
	{
		return;
	}

	if (!MeleeActorInstance && !RangeActorInstance)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	PrewarmStep = 0;
	World->GetTimerManager().SetTimer(
		PrewarmTimerHandle,
		this,
		&UKREquipmentManagerComponent::ExecutePrewarmStep,
		0.1f,  // 0.1초 후 시작
		false
	);
}

void UKREquipmentManagerComponent::ExecutePrewarmStep()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	switch (PrewarmStep)
	{
	case 0:
		if (MeleeActorInstance)
		{
			MeleeActorInstance->PlayEquipEffect();
		}
		PrewarmStep = 1;
		World->GetTimerManager().SetTimer(PrewarmTimerHandle, this, &UKREquipmentManagerComponent::ExecutePrewarmStep, 0.05f, false);
		break;

	case 1:
		if (MeleeActorInstance)
		{
			MeleeActorInstance->PlayUnequipEffect();
		}
		PrewarmStep = 2;
		World->GetTimerManager().SetTimer(PrewarmTimerHandle, this, &UKREquipmentManagerComponent::ExecutePrewarmStep, 0.05f, false);
		break;

	case 2:
		if (RangeActorInstance)
		{
			RangeActorInstance->PlayEquipEffect();
		}
		PrewarmStep = 3;
		World->GetTimerManager().SetTimer(PrewarmTimerHandle, this, &UKREquipmentManagerComponent::ExecutePrewarmStep, 0.05f, false);
		break;

	case 3:
		if (RangeActorInstance)
		{
			RangeActorInstance->PlayUnequipEffect();
		}
		FinishPrewarm();
		break;

	default:
		FinishPrewarm();
		break;
	}
}

void UKREquipmentManagerComponent::FinishPrewarm()
{
	bPrewarmCompleted = true;
	PrewarmStep = 0;

	if (MeleeActorInstance)
	{
		MeleeActorInstance->SetWeaponVisibility(false);
	}
	if (RangeActorInstance)
	{
		RangeActorInstance->SetWeaponVisibility(false);
	}
}

