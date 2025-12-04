#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
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

	FKRInventoryEntry()
		: Instance(nullptr), StackCount(1)
	{}

	FKRInventoryEntry(UKRInventoryItemInstance* InInstance, int32 InStackCount = 1)
		: Instance(InInstance), StackCount(InStackCount)
	{}

	FORCEINLINE void AddCount(int32 InStackCount)
	{
		StackCount += InStackCount;
	}

	FORCEINLINE void SubtractCount(int32 InStackCount)
	{
		if (StackCount < InStackCount)
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

	// 아이템 정의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<UKRInventoryItemDefinition> ItemDefinition;

	// 이 아이템이 가질 Fragment들의 GameplayTag 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FGameplayTagContainer AbilityTags;

	// 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText Description;

	// 최대 스택 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 MaxStackCount = 99;

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
	FKRInventoryList PlayerInventory;
	
	FKRInventoryList& GetInventory();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void InitFragment(FGameplayTag ItemTag);
	
	void InitializeItemDefinitionFragments();
	void InitialFragmentType(TSubclassOf<class UKRInventoryItemFragment> FragmentClass);
};

template <typename T>
void UKRInventorySubsystem::ReceivedAddItemMessage(FGameplayTag Channel, const T& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("AddItemMessage Received! Count: %d"), Message.Count);
}