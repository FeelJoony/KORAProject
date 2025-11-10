#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KRInventorySubsystem.generated.h"

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

	FKRInventoryEntry* CreateItem(FGameplayTag InTag);
	
	void RemoveItem(FGameplayTag InTag);
	
	const FKRInventoryEntry* AddItem(FGameplayTag InTag, int32 StackCount);
	
	void SubtractItem(FGameplayTag InTag, int32 StackCount);
	
	const FKRInventoryEntry* GetItem(FGameplayTag InTag) const;

	TArray<class UKRInventoryItemInstance*> GetAllItems() const;

	TArray<class UKRInventoryItemInstance*> FindAllItemsByTag(FGameplayTag FilterTag) const;

	void Clear();
	
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

	// ========== Fragment Registry ==========
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TSubclassOf<class UKRInventoryItemFragment> GetFragmentClass(FGameplayTag Tag) const;

	// ========== Item Definition Registry ==========
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 LoadItemDefinitionsFromDataTable();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const class UKRInventoryItemDefinition* GetItemDefinition(FGameplayTag ItemTag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const class UKRInventoryItemFragment* GetItemFragment(FGameplayTag ItemTag, FGameplayTag FragmentTag) const;
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<FGameplayTag> GetAllRegisteredItemTags() const;

	// ========== Player Inventory ==========

	UFUNCTION(BlueprintCallable, Category = Inventory)
	class UKRInventoryItemInstance* AddItem(APlayerState* Owner, FGameplayTag ItemTag, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SubtractItem(APlayerState* Owner, FGameplayTag ItemTag, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RemoveItem(APlayerState* Owner, FGameplayTag InTag);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const UKRInventoryItemInstance* GetItem(APlayerState* Owner, FGameplayTag InTag);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetItemCountByTag(APlayerState* Owner, FGameplayTag InTag);
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<class UKRInventoryItemInstance*> GetAllItems(class APlayerState* Owner) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<class UKRInventoryItemInstance*> FindItemsByTag(class APlayerState* Owner, FGameplayTag ItemTag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void ClearInventory(APlayerState* Owner);

private:
	UPROPERTY()
	TMap<FGameplayTag, TSubclassOf<class UKRInventoryItemFragment>> FragmentRegistry;

	UPROPERTY()
	TMap<TObjectPtr<class APlayerState>, FKRInventoryList> PlayerInventories;
	
	FKRInventoryList& GetOrCreateInventory(class APlayerState* Owner);

	void InitializeItemDefinitionFragments(const FGameplayTagContainer& AbilityTags);
	void InitialFragmentType(TSubclassOf<class UKRInventoryItemFragment> FragmentClass);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RegisterFragmentClass(FGameplayTag Tag, TSubclassOf<class UKRInventoryItemFragment> FragmentClass);
};
