#include "KREquipmentManagerComponent.h"
#include "KREquipmentDefinition.h"
#include "KREquipmentInstance.h"
#include "Weapons/KRWeaponInstance.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IDetailTreeNode.h"
#include "Components/KRPawnExtensionComponent.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "GameFramework/Character.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KRWeaponAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentManagerComponent)

DEFINE_LOG_CATEGORY(LogEquipmentManagerComponent);

struct FAttributeMappingPair
{
	FGameplayAttribute WeaponAttribute; // 장비 데이터 (Source)
	FGameplayAttribute PlayerAttribute; // 플레이어 적용 대상 (Target)
};

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
}

void UKREquipmentManagerComponent::OnAbilitySystemInitialized()
{
	// 중복 호출 방지
	if (bASCInitCallbackProcessed)
	{
		return;
	}
	bASCInitCallbackProcessed = true;

	SpawnActorInstances();
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
	const FKRAppliedEquipmentEntry& Entry = FindEntryByTag(ItemTag);
	if (!Entry.IsValid())
	{
		return;
	}
	Entry.ItemInstance = ItemInstance;

	ACharacter* Character = CastChecked<ACharacter>(GetOwner());

	// 스탯 적용
	const UInventoryFragment_SetStats* SetStatFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_SetStats>();
	if (SetStatFragment)
	{
		ApplyStatsToASC(SetStatFragment, true);
	}

	// 무기 아이템인 경우: 무기 액터에 아이템 인스턴스 연결 (GA/IMC/AnimLayer는 ActivateCombatMode에서 처리)
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
		UE_LOG(LogEquipmentManagerComponent, Log, TEXT("EquipItem: Configured weapon for ItemTag [%s]"), *ItemTag.ToString());
	}

	// 코스튬 장비 처리
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

	// 스탯 제거
	const UInventoryFragment_SetStats* SetStatFragment = ItemInstance->FindFragmentByClass<UInventoryFragment_SetStats>();
	if (SetStatFragment)
	{
		ApplyStatsToASC(SetStatFragment, false);
	}

	// 현재 활성화된 무기가 이 아이템이라면 전투 모드 비활성화
	if (bCombatModeActive && ActiveWeaponTypeTag.MatchesTag(ItemTag))
	{
		DeactivateCombatMode();
	}

	Entry.ItemInstance = nullptr;
	UE_LOG(LogEquipmentManagerComponent, Log, TEXT("UnequipItem: Unequipped ItemTag [%s]"), *ItemTag.ToString());
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

	// Fragment가 들고 있는 무기 스탯 데이터셋 가져오기
	const UKRWeaponAttributeSet* SourceWeaponStats = SetStatsFragment->GetWeaponAttributeSet();
	if (!SourceWeaponStats) return;

	static const TArray<FAttributeMappingPair> StatMappings = {
		// 1. 공격력 (매핑 가능)
		{ UKRWeaponAttributeSet::GetAttackPowerAttribute(), UKRCombatCommonSet::GetAttackPowerAttribute() },

		// [확장 가이드] KRCombatCommonSet에 해당 스탯들을 추가한 뒤 아래 주석을 해제
		/*
		{ UKRWeaponAttributeSet::GetDefensePowerAttribute(), UKRCombatCommonSet::GetDefensePowerAttribute() }, // 방어력
		{ UKRWeaponAttributeSet::GetAttackSpeedAttribute(), UKRCombatCommonSet::GetAttackSpeedAttribute() },   // 공격 속도
		{ UKRWeaponAttributeSet::GetCritChanceAttribute(), UKRCombatCommonSet::GetCritChanceAttribute() },     // 치명타 확률
		{ UKRWeaponAttributeSet::GetCritMultiAttribute(), UKRCombatCommonSet::GetCritMultiAttribute() },       // 치명타 배율
		*/
	};

	// [Loop Logic] 정의된 매핑 테이블을 순회하며 스탯 자동 적용
	for (const FAttributeMappingPair& Pair : StatMappings)
	{
		// 1. 유효성 검사: 무기 스탯과 플레이어 스탯 속성이 모두 유효해야 함
		if (Pair.WeaponAttribute.IsValid() && Pair.PlayerAttribute.IsValid())
		{
			// 2. 무기 데이터에서 값 추출 (Source)
			float StatValue = Pair.WeaponAttribute.GetNumericValue(SourceWeaponStats);

			// 값이 0에 가까우면 연산 생략 (최적화)
			if (FMath::IsNearlyZero(StatValue))
			{
				continue;
			}

			// 3. 플레이어 ASC의 기본값(Base) 수정 (Target)
			// 참고: 다이어그램의 ApplyMod(Modifier) 대신 현재 구조에 맞춰 SetBase 사용
			float CurrentBase = ASC->GetNumericAttributeBase(Pair.PlayerAttribute);
			float NewBase = bAdd ? (CurrentBase + StatValue) : (CurrentBase - StatValue);

			ASC->SetNumericAttributeBase(Pair.PlayerAttribute, NewBase);

			// 로그 (디버깅용)
			UE_LOG(LogEquipmentManagerComponent, Verbose, TEXT("Applied Stat: %s -> Value: %f (Add: %d)"), *Pair.PlayerAttribute.GetName(), StatValue, bAdd);
		}
	}
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
	// 이미 스폰되었다면 중복 호출 방지
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

	// 스폰 후 캐릭터에 Attach
	AttachWeaponsToCharacter();
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

			// 이미 장착된 아이템이 있으면 ConfigureWeapon 호출 (초기화 순서 문제 해결)
			if (MeleeEntry.ItemInstance)
			{
				MeleeActorInstance->ConfigureWeapon(MeleeEntry.ItemInstance);
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

			// 이미 장착된 아이템이 있으면 ConfigureWeapon 호출 (초기화 순서 문제 해결)
			if (RangeEntry.ItemInstance)
			{
				RangeActorInstance->ConfigureWeapon(RangeEntry.ItemInstance);
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

	UE_LOG(LogEquipmentManagerComponent, Log, TEXT("DeactivateCombatMode: Combat mode deactivated"));
}

