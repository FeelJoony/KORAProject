#pragma once

#include "CoreMinimal.h"
//#include "System/GameplayTagStack.h"
#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KRInventoryItemInstance.generated.h"

class FLifetimeProperty;

class UKRInventoryItemDefinition;
class UKRInventoryItemFragment;
struct FFrame;
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

	class UKRInventoryItemDefinition* GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, Category = Inventory)
	UKRInventoryItemInstance* CreateItemDefinition();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	const UKRInventoryItemFragment* FindFragmentByTag(FGameplayTag Tag) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByTag(FGameplayTag Tag) const
	{
		return Cast<ResultClass>(FindFragmentByTag(Tag));
	}

	TArray<FGameplayTag> GetAllFragmentTags() const;

private:
	void SetItemDef(class UKRInventoryItemDefinition* InDef);

	friend struct FKRInventoryList;
	friend class UKRInventorySubsystem;

private:
	//UPROPERTY()
	//FGameplayTagStackContainer StatTags;

	UPROPERTY()
	TObjectPtr<class UKRInventoryItemDefinition> ItemDef;
};
