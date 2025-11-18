#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "KRInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

UCLASS(MinimalAPI, Abstract)
class UKRInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(class UKRInventoryItemInstance* Instance) {}

	UFUNCTION(BlueprintCallable, Category = "Fragment")
	virtual FGameplayTag GetFragmentTag() const { return FGameplayTag{}; }
};

UCLASS(Blueprintable)
class UKRInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UKRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	static UKRInventoryItemDefinition* CreateItemDefinition()
	{
		return NewObject<UKRInventoryItemDefinition>();
	}

	UFUNCTION(BlueprintCallable, Category = Inventory)
	UKRInventoryItemFragment* FindFragmentByTag(FGameplayTag Tag);
	
	//UFUNCTION(BlueprintCallable, Category = Inventory)
	const UKRInventoryItemFragment* FindFragmentByTag(FGameplayTag Tag) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByTag(FGameplayTag Tag)
	{
		return Cast<ResultClass>(FindFragmentByTag(Tag));
	}
	
	template <typename ResultClass>
	const ResultClass* FindFragmentByTag(FGameplayTag Tag) const
	{
		return Cast<ResultClass>(FindFragmentByTag(Tag));
	}

	void AddFragment(FGameplayTag Tag, class UKRInventoryItemFragment* Fragment);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<FGameplayTag> GetAllFragmentTags() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FORCEINLINE FText GetDisplayName() const { return DisplayName; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Fragments)
	TMap<FGameplayTag, TObjectPtr<UKRInventoryItemFragment>> FragmentContainer;
};
