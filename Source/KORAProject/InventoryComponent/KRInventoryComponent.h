#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Item/KRBaseItem.h"
#include "KRInventoryComponent.generated.h"

class UKRWeaponItem;
class UKRModuleItem;
class UKREquipItem;
class UKRConsumeItem;

USTRUCT(BlueprintType)
struct FKRBaseItemArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UKRBaseItem>> Items;
};

USTRUCT(BlueprintType)
struct FKRWeaponItemArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UKRWeaponItem>> Items;
};

USTRUCT(BlueprintType)
struct FKRModuleItemArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UKRModuleItem>> Items;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KORAPROJECT_API UKRInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UKRInventoryComponent();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<TObjectPtr<UKRBaseItem>> AllItems;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TMap<FGameplayTag, TObjectPtr<UKREquipItem>> EquippedItems;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TMap<int32, TObjectPtr<UKRConsumeItem>> QuickSlots;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, FKRBaseItemArrayWrapper> ItemsByCategory;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FKRWeaponItemArrayWrapper> ItemsByWeaponType;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FKRModuleItemArrayWrapper> ItemsByModuleType;

protected:
	void IndexItem(UKRBaseItem* InNewItem);
	void UnindexItem(UKRBaseItem* InItem);
	void RebuildIndex();
	UKRBaseItem* FindExistingStack(UKRBaseItem* InItemToFind) const;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UKRBaseItem* InNewItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UKRBaseItem* InItem, int32 InCount = 1);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UKRBaseItem*> GetItemsByTag(FGameplayTag InFilterTag) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UKRBaseItem*> GetItemsByTagsAll(const TArray<FGameplayTag>& InFilterTags) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UKRBaseItem*> GetItemsByTagsAny(const TArray<FGameplayTag>& InFilterTags) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UKRModuleItem*> GetCompatibleModules(UKRWeaponItem* InWeapon) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool EquipItem(UKREquipItem* InItem, FGameplayTag InSlotTag);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UnequipItem(FGameplayTag InSlotTag);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RegisterQuickSlot(int32 InSlotIndex, UKRConsumeItem* InItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseQuickSlot(int32 InSlotIndex);
};