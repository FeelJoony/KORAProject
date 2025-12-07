#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "KRInventorySubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventorySubSystem, Log, All);

class UKRInventoryItemInstance;
class UKRInventoryItemDefinition;
class UKRInventoryItemFragment;

USTRUCT(BlueprintType)
struct FAddItemMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Message)
	int32 StackCount;
};

USTRUCT(BlueprintType)
struct FKRInventoryEntry
{
	GENERATED_BODY()

	static constexpr int32 GlobalMaxStackCount = 99;
	
	FKRInventoryEntry()
		: Instance(nullptr), StackCount(1)
	{ StackCount = FMath::Clamp(StackCount, 0, GlobalMaxStackCount); }

	FKRInventoryEntry(UKRInventoryItemInstance* InInstance, int32 InStackCount = 1)
		: Instance(InInstance), StackCount(InStackCount)
	{ StackCount = FMath::Clamp(StackCount, 0, GlobalMaxStackCount); }

	FORCEINLINE void AddCount(int32 InStackCount)
	{
		if (InStackCount <= 0)
		{
			return;
		}

		const int32 NewCount = StackCount + InStackCount;
		StackCount = FMath::Clamp(NewCount, 0, GlobalMaxStackCount);
	}

	FORCEINLINE void SubtractCount(int32 InStackCount)
	{
		if (InStackCount <= 0)
		{
			return;
		}
		
		if (StackCount <= InStackCount)
		{
			StackCount = 0;
			return;
		}

		StackCount -= InStackCount;
	}

	FORCEINLINE int32 GetStackCount() const
	{
		return StackCount;
	}

	FORCEINLINE class UKRInventoryItemInstance* GetItemInstance() const
	{
		return Instance;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UKRInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 StackCount = 1;
};

USTRUCT(BlueprintType)
struct FKRInventoryList
{
	GENERATED_BODY()

	FKRInventoryList() {}

	UObject* OwnerContext = nullptr;

	void SetOwnerContext(UObject* InOwner) { OwnerContext = InOwner; }
	
	FKRInventoryEntry* CreateItem(FGameplayTag InTag);
	void RemoveItem(FGameplayTag InTag);
	const FKRInventoryEntry* AddItem(FGameplayTag InTag, int32 StackCount, bool& OutIsNew);
	void SubtractItem(FGameplayTag InTag, int32 StackCount);
	FKRInventoryEntry* GetItem(FGameplayTag InTag);
	const FKRInventoryEntry* GetItem(FGameplayTag InTag) const;
	TArray<class UKRInventoryItemInstance*> GetAllItems() const;
	TArray<class UKRInventoryItemInstance*> FindAllItemsByTag(FGameplayTag FilterTag) const;
	void Clear();

	void AddEntryDirect(UKRInventoryItemInstance* NewInstance);
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, FKRInventoryEntry> Entries;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer ItemTagContainer;
};

USTRUCT(BlueprintType)
struct FKRItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<UKRInventoryItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FGameplayTagContainer AbilityTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 MaxStackCount;

	FKRItemTableRow()
		: ItemDefinition(nullptr)
		, MaxStackCount(99)
	{}
};

UCLASS()
class KORAPROJECT_API UKRInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TSubclassOf<class UKRInventoryItemFragment> GetFragmentClass(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void AddFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const class UKRInventoryItemDefinition* GetItemDefinition(FGameplayTag ItemTag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const class UKRInventoryItemFragment* GetItemFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag) const;
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<FGameplayTag> GetAllRegisteredItemTags() const;
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	class UKRInventoryItemInstance* AddItem(FGameplayTag ItemTag, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SubtractItem(FGameplayTag ItemTag, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RemoveItem(FGameplayTag InTag);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const UKRInventoryItemInstance* GetItem(FGameplayTag InTag);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetItemCountByTag(FGameplayTag InTag);
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<class UKRInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<class UKRInventoryItemInstance*> FindItemsByTag(FGameplayTag ItemTag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void ClearInventory();
	
	void InitializeQuickSlots();
	
	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	bool BindItemByQuickSlotTag(FGameplayTag SlotTag, FGameplayTag ItemTag);
	
	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	FGameplayTag GetQuickSlotItemTag(FGameplayTag SlotTag) const;
	
	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	UKRInventoryItemInstance* GetQuickSlotItemInstance(FGameplayTag SlotTag) const;

	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	bool GetQuickSlotData(FGameplayTag SlotTag, FGameplayTag& OutItemTag, int32& OutQuantity) const;

	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	UAbilitySystemComponent* GetPlayerASC() const;
	
	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	bool UseQuickSlotItem(FGameplayTag SlotTag);
	
	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	FGameplayTag GetCurrentSelectedQuickSlot() const { return SelectedQuickSlot; }
	
	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	bool SelectQuickSlotClockwise();
	
	UFUNCTION(BlueprintCallable, Category="QuickSlot")
	bool SelectQuickSlotCounterClockwise();

	UFUNCTION(BlueprintCallable, Category="Inventory|Consumable")
	bool CanUseConsumableItem(FGameplayTag ItemTag, UAbilitySystemComponent* TargetASC, FText& OutReason) const;

	UFUNCTION(BlueprintCallable, Category="Inventory|Consumable")
	float GetConsumableCooldownRemaining(FGameplayTag ItemTag, UAbilitySystemComponent* TargetASC) const;

	UFUNCTION(BlueprintCallable, Category="Inventory|Consumable")
	int32 GetQuickSlotDisplayCount(const FGameplayTag& ItemTag, int32 ActualCount) const;
	
	UFUNCTION(BlueprintCallable, Category="Inventory|Consumable")
	bool UseConsumableItem(FGameplayTag ItemTag, UAbilitySystemComponent* TargetASC);

	
	UFUNCTION()
	void OnMessageReceived(const FGameplayTag Channel, const FAddItemMessage& Message);
	
	template<typename T>
	void ReceivedAddItemMessage(FGameplayTag Channel, const T& Message);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void AddItemInstance(UKRInventoryItemInstance* InInstance);
	
private:
	UPROPERTY()
	TMap<FGameplayTag, TSubclassOf<class UKRInventoryItemFragment>> FragmentRegistry;
	
	UPROPERTY()
	TMap<FGameplayTag, FGameplayTag> PlayerQuickSlot;

	UPROPERTY()
	TMap<FGameplayTag, int32> PlayerQuickSlotStacks;
	
	UPROPERTY()
	FGameplayTag SelectedQuickSlot;
	
	UPROPERTY()
	FKRInventoryList PlayerInventory;
	
	FKRInventoryList& GetInventory();
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void InitFragment(FGameplayTag ItemTag);
	
	void InitializeItemDefinitionFragments();
	void InitialFragmentType(TSubclassOf<class UKRInventoryItemFragment> FragmentClass);
	
	bool IsPersistentQuickSlotItem(const FGameplayTag& ItemTag) const;
	bool IsAssignedQuickSlot() const;
	bool IsAssignedSelectedQuickSlot() const;
	
	bool SelectQuickSlotInternal(bool bClockwise);
	void NotifySlotChanged(const FGameplayTag& NewSelectedSlot);
	
	void SendQuickSlotMessage(
		EQuickSlotAction ActionType,
		const FGameplayTag& SlotTag,
		const FGameplayTag& ItemTag,
		int32 ItemQuantity,
		float DurationSeconds = 0.f
	);
	
	FGameplayTag MapSlotTagToUISlotTag(const FGameplayTag& SlotTag) const;

	bool SelectNextAvailableQuickSlot();
	void NotifyQuickSlotQuantityChanged(const FGameplayTag& ItemTag);
	
	bool IsValidQuickSlot(const FGameplayTag& SlotTag) const;
	bool HasQuickSlotItemFragment(FGameplayTag ItemTag, UKRInventoryItemInstance*& OutInstance);
	
	const FKRInventoryEntry* FindEntryByItemTag(const FGameplayTag& ItemTag) const;
	UKRInventoryItemInstance* FindInstanceByItemTag(const FGameplayTag& ItemTag) const;
	
	bool HandleQuickSlotUnbind(FGameplayTag SlotTag, FGameplayTag ItemTag);
	void UnbindExistingItemBySlotTag(FGameplayTag SlotTag, FGameplayTag ItemTag);
	void HandleAutoSelectSlotFirstRegisteredItem(FGameplayTag SlotTag, bool bHadAssignedBefore);
	
	int32 GetItemQuantity_Internal(const FGameplayTag& ItemTag) const;
	int32 GetConsumableStackMaxForItem(const FGameplayTag& ItemTag) const;
	
	void OnConfirmMessage(FGameplayTag MessageTag, const FKRUIMessage_Confirm& Payload);
};

template <typename T>
void UKRInventorySubsystem::ReceivedAddItemMessage(FGameplayTag Channel, const T& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("AddItemMessage Received! Count: %d"), Message.StackCount);
}
