// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTag/KRUITag.h"
#include "KRUIMessagePayloads.generated.h"

// -----  Message Tag Helper -----

struct KORAPROJECT_API FKRUIMessageTags
{
	static const FGameplayTag ProgressBar() { return KRTAG_UI_MESSAGE_PROGRESSBAR; }
	static const FGameplayTag GreyHP() { return KRTAG_UI_MESSAGE_GREYHP; }
	static const FGameplayTag ItemLog() { return KRTAG_UI_MESSAGE_ITEMLOG; }
	static const FGameplayTag Weapon() { return KRTAG_UI_MESSAGE_WEAPON; }
	static const FGameplayTag QuickSlot() { return KRTAG_UI_MESSAGE_QUICKSLOT; }
	static const FGameplayTag Quest() { return KRTAG_UI_MESSAGE_QUEST; }
	static const FGameplayTag Currency() { return KRTAG_UI_MESSAGE_CURRENCY; }
	static const FGameplayTag Confirm() { return KRTAG_UI_MESSAGE_CONFIRM; }
	static const FGameplayTag Tutorial() { return KRTAG_UI_MESSAGE_TUTORIAL; }
};

 // -----  ProgressBar -----

UENUM(BlueprintType)
enum class EProgressBarType : uint8
{
	MainHP			UMETA(DisplayName = "Main HP"),
	GreyHP			UMETA(DisplayName = "Grey HP"),
	Stamina			UMETA(DisplayName = "Stamina"),
	CoreDrive		UMETA(DisplayName = "Core Drive")
};

UENUM(BlueprintType)
enum class EProgressUpdateMode : uint8
{
	Instant			UMETA(DisplayName = "Instant"),
	Animated		UMETA(DisplayName = "Animated")
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Progress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) EProgressBarType BarType = EProgressBarType::MainHP;
	UPROPERTY(BlueprintReadOnly) EProgressUpdateMode UpdateMode = EProgressUpdateMode::Animated;

	UPROPERTY(BlueprintReadOnly) float NewValue = 0.f;
	UPROPERTY(BlueprintReadOnly) float MaxValue = 100.f;
	UPROPERTY(BlueprintReadOnly) float Delta = 0.f; // + Recovery, - Damage

	UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> TargetActor;
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_GreyHP
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) bool bRecovered = false;

	UPROPERTY(BlueprintReadOnly) float RecoveredAmount = 0.f;
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

	UPROPERTY(BlueprintReadOnly) EWeaponMessageAction Action = EWeaponMessageAction::Equipped;
	UPROPERTY(BlueprintReadOnly) FGameplayTag WeaponTypeTag;

	UPROPERTY(BlueprintReadOnly) int32 CurrentAmmo = 0;
	UPROPERTY(BlueprintReadOnly) int32 MaxAmmo = 0;
};

// -----  Item Acquisition -----

UENUM(BlueprintType)
enum class EItemAcquisitionType : uint8
{
	NormalItem			UMETA(DisplayName = "New Item Acquired"), // Item, Currency
	QuestItem		UMETA(DisplayName = "Quest Item Acquired") // Quest Item
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_ItemAcquisition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) EItemAcquisitionType AcquisitionType = EItemAcquisitionType::NormalItem;

	UPROPERTY(BlueprintReadOnly) FName ItemNameKey;
	UPROPERTY(BlueprintReadOnly) int32 Quantity = 1;
	UPROPERTY(BlueprintReadOnly) FGameplayTag ItemRarityTag;
};

// -----  Quickslot -----

UENUM(BlueprintType)
enum class EQuickSlotDirection : uint8
{
	North			UMETA(DisplayName = "North (Up)"),
	East			UMETA(DisplayName = "East (Right)"),
	South			UMETA(DisplayName = "South (Down)"),
	West			UMETA(DisplayName = "West (Left)")
};

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

	UPROPERTY(BlueprintReadOnly) EQuickSlotDirection CurrentlySelectedSlot = EQuickSlotDirection::North;
	UPROPERTY(BlueprintReadOnly) FSoftObjectPath ItemIconPath;
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
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRUIMessage_Confirm
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) EConfirmContext Context = EConfirmContext::Generic;
	UPROPERTY(BlueprintReadOnly) EConfirmResult Result = EConfirmResult::None;
	UPROPERTY(BlueprintReadOnly) FGameplayTag ItemTag; // ItemTag, WeaponTag, etc.
	UPROPERTY(BlueprintReadOnly) int32 Quantity = 0; // Item Quantity
};

// ----- Tutorial -----
USTRUCT(BlueprintType)
struct FKRUIMessage_Tutorial
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FName TutorialDTRowName;
};