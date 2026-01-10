// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTag/KRUITag.h"
#include "KRUIMessagePayloads.generated.h"

// -----  Message Tag Helper -----

struct KORAPROJECT_API FKRUIMessageTags
{
	static const FGameplayTag Guard() { return KRTAG_UI_MESSAGE_GUARD; }
	static const FGameplayTag ItemLog() { return KRTAG_UI_MESSAGE_ITEMLOG; }
	static const FGameplayTag Weapon() { return KRTAG_UI_MESSAGE_WEAPON; }
	static const FGameplayTag QuickSlot() { return KRTAG_UI_MESSAGE_QUICKSLOT; }
	static const FGameplayTag EquipSlot() { return KRTAG_UI_MESSAGE_EQUIPSLOT; }
	static const FGameplayTag Quest() { return KRTAG_UI_MESSAGE_QUEST; }
	static const FGameplayTag Currency() { return KRTAG_UI_MESSAGE_CURRENCY; }
	static const FGameplayTag ShopStockUpdated() { return KRTAG_UI_MESSAGE_SHOPSTOCKUPDATED; }
	static const FGameplayTag Confirm() { return KRTAG_UI_MESSAGE_CONFIRM; }
	static const FGameplayTag Tutorial() { return KRTAG_UI_MESSAGE_TUTORIAL; }
	static const FGameplayTag EquipmentUI() { return KRTAG_UI_MESSAGE_EQUIPMENTUI; }
	static const FGameplayTag SaveDeathLevelInfo() { return KRTAG_UI_MESSAGE_INFO; }

	static const FGameplayTag QuickSlot_North() { return KRTAG_UI_QUICKSLOT_NORTH; }
	static const FGameplayTag QuickSlot_East() { return KRTAG_UI_QUICKSLOT_EAST; }
	static const FGameplayTag QuickSlot_South() { return KRTAG_UI_QUICKSLOT_SOUTH; }
	static const FGameplayTag QuickSlot_West() { return KRTAG_UI_QUICKSLOT_WEST; }
};

 // -----  Guard -----

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Guard
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) TWeakObjectPtr<AActor> TargetActor;
	UPROPERTY(BlueprintReadWrite) bool bGuardSuccess = false;
	UPROPERTY(BlueprintReadWrite) bool bPerfectGuard = false;
};

// -----  Weapon  -----

UENUM(BlueprintType)
enum class EWeaponMessageAction : uint8
{
	Equipped    UMETA(DisplayName = "Equipped"),
	Unequipped  UMETA(DisplayName = "Unequipped"),
	Switched    UMETA(DisplayName = "Switched"),
	AmmoUpdated UMETA(DisplayName = "AmmoUpdated")
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Weapon
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) EWeaponMessageAction Action = EWeaponMessageAction::Equipped;
	UPROPERTY(BlueprintReadWrite) FGameplayTag WeaponTypeTag;

	UPROPERTY(BlueprintReadWrite) int32 CurrentAmmo = 0;
	UPROPERTY(BlueprintReadWrite) int32 MaxAmmo = 0;
};

// -----  Item Log -----

UENUM(BlueprintType)
enum class EItemLogEvent : uint8
{
	AcquiredNormal   UMETA(DisplayName = "New Item Acquired"),
	AcquiredQuest    UMETA(DisplayName = "Quest Item Acquired"),
	Consumed         UMETA(DisplayName = "Item Consumed"),
	Sold             UMETA(DisplayName = "Item Sold")
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_ItemLog
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) EItemLogEvent EventType = EItemLogEvent::AcquiredNormal;
	UPROPERTY(BlueprintReadOnly) FGameplayTag ItemTag;
	UPROPERTY(BlueprintReadOnly) int32 AcquiredQuantity = 0;
	UPROPERTY(BlueprintReadOnly) int32 NewTotalQuantity = 0;
	UPROPERTY(BlueprintReadOnly) FName ItemNameKey;
};

// -----  Quickslot -----

UENUM(BlueprintType)
enum class EQuickSlotAction : uint8
{
	ItemRegistered		UMETA(DisplayName = "Item Registered"),
	ItemUnregistered	UMETA(DisplayName = "Item Unregistered"),
	ItemUsed			UMETA(DisplayName = "Item Used"),
	SlotChanged			UMETA(DisplayName = "Slot Changed"),
	QuantityUpdated		UMETA(DisplayName = "Quantity Updated")
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_QuickSlot
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) EQuickSlotAction ActionType = EQuickSlotAction::ItemRegistered;
	UPROPERTY(BlueprintReadOnly) FName ItemNameKey;
	UPROPERTY(BlueprintReadOnly) int32 ItemQuantity = 0;
	UPROPERTY(BlueprintReadOnly) float Duration = 3.f; // Only When ItemUsed

	UPROPERTY(BlueprintReadOnly) FGameplayTag CurrentlySelectedSlot = FKRUIMessageTags::QuickSlot_North();
	UPROPERTY(BlueprintReadOnly) TSoftObjectPtr<UTexture2D> ItemIcon;
};

// -----  Equipslot -----

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_EquipSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SlotTag;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UKRInventoryItemInstance> ItemInstance;
};

// -----  Quest -----

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Quest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName QuestNameKey;
};

// ----- Currency ----- 

USTRUCT(BlueprintType) 
struct KORAPROJECT_API FKRUIMessage_Currency
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 CurrentGearing = 0;

	UPROPERTY(BlueprintReadOnly) int32 DeltaGearing = 0; // + Acq - Lost

	UPROPERTY(BlueprintReadOnly) int32 CurrentCorbyte = 0;
};

// ----- Shop Stock Updated-----
USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_ShopStockUpdated
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 ShopRevision = 0;
};

//----- Confirm Modal -----

UENUM(BlueprintType)
enum class EConfirmResult : uint8
{
	None    UMETA(DisplayName = "None"),
	Yes     UMETA(DisplayName = "Yes"),
	No      UMETA(DisplayName = "No"),
};

UENUM(BlueprintType)
enum class EConfirmContext : uint8
{
	Generic     UMETA(DisplayName = "Generic"),
	NewGame     UMETA(DisplayName = "MainMenu|NewGame"),
	Continue	UMETA(DisplayName = "MainMenu|Continue"),
	QuitGame    UMETA(DisplayName = "Menu|QuitGame"),
	ShopBuy     UMETA(DisplayName = "Shop|Buy"),
	ShopSell    UMETA(DisplayName = "Shop|Sell"),
	WeaponUpgrade   UMETA(DisplayName = "BlackSmith|WeaponUpgrade"),
	QuickSlotAssign UMETA(DisplayName = "Inventory|QuickSlotAssign"),
	QuickSlotRemove UMETA(DisplayName = "Inventory|QuickSlotRemove"),
	InventoryItemUse UMETA(DisplayName = "Inventory|ItemUse"),
	Equipment		UMETA(DisplayName = "Equipment|Equipped"),
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Confirm
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) EConfirmContext Context = EConfirmContext::Generic;
	UPROPERTY(BlueprintReadOnly) EConfirmResult Result = EConfirmResult::None;
	UPROPERTY(BlueprintReadOnly) FGameplayTag ItemTag; // ItemTag, WeaponTag, etc.
	UPROPERTY(BlueprintReadOnly) FGameplayTag SlotTag; // Only for QuickSlot Assign
	UPROPERTY(BlueprintReadOnly) int32 Quantity = 0; // Item Quantity
};

// ----- Tutorial -----
USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Tutorial
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FName TutorialDTRowName;
};

// ----- Equipment UI -----
USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_EquipmentUI
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) bool bIsOpen = false;
};

// ----- Save / Death / LevelTransition -----
UENUM(BlueprintType)
enum class EInfoContext : uint8
{
	SaveGame UMETA(DisplayName = "SaveGame"),
	Death UMETA(DisplayName = "Death"),
	LevelTransition UMETA(DisplayName = "LevelTransition"),
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Info
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) EInfoContext Context = EInfoContext::SaveGame;
	UPROPERTY(BlueprintReadWrite) FString StringTableKey = TEXT("");
};