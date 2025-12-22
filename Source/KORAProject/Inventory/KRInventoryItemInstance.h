#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KRInventoryItemInstance.generated.h"

class UKRInventoryItemDefinition;
class UKRInventoryItemFragment;
struct FGameplayTag;

UCLASS()
class KORAPROJECT_API UKRInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UKRInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UKRInventoryItemInstance* CreateItemInstance()
	{
		return NewObject<UKRInventoryItemInstance>();
	}

	FORCEINLINE void SetItemTag(FGameplayTag InTag) { ItemTag = InTag; }
	FORCEINLINE FGameplayTag GetItemTag() const { return ItemTag; }

	void SetOwnerContext(UObject* InOwner) { OwnerContext = InOwner; }
	UObject* GetOwnerContext() const { return OwnerContext; }

	UKRInventoryItemDefinition* GetItemDef() const { return ItemDef; }

	UFUNCTION(BlueprintCallable, Category = Inventory)
	UKRInventoryItemInstance* CreateItemDefinition();
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UKRInventoryItemFragment* FindFragmentByClass(TSubclassOf<UKRInventoryItemFragment> FragmentClass) const;
	
	UKRInventoryItemFragment* FindInstanceFragmentByClass(TSubclassOf<UKRInventoryItemFragment> FragmentClass);

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return Cast<ResultClass>(FindFragmentByClass(ResultClass::StaticClass()));
	}

	template <typename ResultClass>
	ResultClass* FindInstanceFragmentByClass()
	{
		return Cast<ResultClass>(FindInstanceFragmentByClass(ResultClass::StaticClass()));
	}

	const UKRInventoryItemFragment* FindFragmentByTag(FGameplayTag Tag) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByTag(FGameplayTag Tag) const
	{
		return Cast<ResultClass>(FindFragmentByTag(Tag));
	}

	TArray<FGameplayTag> GetAllFragmentTags() const;

	void SetItemDef(UKRInventoryItemDefinition* InDef);
	
	void AddInstanceFragment(FGameplayTag Tag, UKRInventoryItemFragment* Fragment);

private:
	friend struct FKRInventoryList;
	friend class UKRInventorySubsystem;
	
	void DuplicateFragmentsFromDefinition();

private:
	UPROPERTY()
	FGameplayTag ItemTag;

	UPROPERTY()
	TObjectPtr<UObject> OwnerContext;
	
	UPROPERTY()
	TObjectPtr<UKRInventoryItemDefinition> ItemDef;
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UKRInventoryItemFragment>> InstanceFragments;
};

