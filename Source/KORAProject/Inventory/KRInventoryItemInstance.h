#pragma once

#include "CoreMinimal.h"
//#include "System/GameplayTagStack.h"
#include "Templates/SubclassOf.h"
#include "Subsystems/GameInstanceSubsystem.h"
//#include "KRInventoryItemInstance.generated.h"

class FLifetimeProperty;

class UKRInventoryItemDefinition;
class UKRInventoryItemFragment;
struct FFrame;
struct FGameplayTag;

//UCLASS()
//class KORAPROJECT_API UKRInventoryItemInstance : public UGameInstanceSubsystem
//{
//	GENERATED_BODY()
//	
//public:
//	UKRInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	~UObject interface
//	virtual bool IsSupportedForNetworking() const override { return true; }
//	~End of UObject interface
//
//	 Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
//	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);
//
//	 Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
//	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);
//
//	 Returns the stack count of the specified tag (or 0 if the tag is not present)
//	UFUNCTION(BlueprintCallable, Category = Inventory)
//	int32 GetStatTagStackCount(FGameplayTag Tag) const;
//
//	 Returns true if there is at least one stack of the specified tag
//	UFUNCTION(BlueprintCallable, Category = Inventory)
//	bool HasStatTag(FGameplayTag Tag) const;
//
//	TSubclassOf<UKRInventoryItemDefinition> GetItemDef() const
//	{
//		return ItemDef;
//	}
//
//	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
//	const UKRInventoryItemFragment* FindFragmentByClass(TSubclassOf<UKRInventoryItemFragment> FragmentClass) const;
//
//	template <typename ResultClass>
//	const ResultClass* FindFragmentByClass() const
//	{
//		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
//	}
//
//private:
//	void SetItemDef(TSubclassOf<UKRInventoryItemDefinition> InDef);
//
//	friend struct FKRInventoryList;
//
//private:
//	UPROPERTY()
//	FGameplayTagStackContainer StatTags;
//
//	 The item definition
//	UPROPERTY()
//	TSubclassOf<UKRInventoryItemDefinition> ItemDef;
//};
