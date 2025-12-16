#include "Inventory/KRInventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventoryItemDefinition)

UKRInventoryItemDefinition::UKRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const class UKRInventoryItemFragment* UKRInventoryItemDefinition::FindFragmentByClass(
	TSubclassOf<class UKRInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (const auto& FragmentPair : FragmentContainer)
		{
			if (FragmentPair.Value && FragmentPair.Value->IsA(FragmentClass))
			{
				return FragmentPair.Value;
			}
		}
	}

	return nullptr;
}

UKRInventoryItemFragment* UKRInventoryItemDefinition::FindFragmentByTag(FGameplayTag Tag)
{
	if (Tag.IsValid())
	{
		return FragmentContainer.FindRef(Tag);
	}
	return nullptr;
}

const UKRInventoryItemFragment* UKRInventoryItemDefinition::FindFragmentByTag(FGameplayTag Tag) const
{
	if (Tag.IsValid())
	{
		if (const auto FoundFragment = FragmentContainer.FindRef(Tag))
		{
			return FoundFragment;
		}
	}
	return nullptr;
}

void UKRInventoryItemDefinition::AddFragment(FGameplayTag Tag, UKRInventoryItemFragment* Fragment)
{
	if (Tag.IsValid() && Fragment)
	{
		FragmentContainer.Add(Tag, Fragment);
	}
}

TArray<FGameplayTag> UKRInventoryItemDefinition::GetAllFragmentTags() const
{
	TArray<FGameplayTag> Tags;
	FragmentContainer.GetKeys(Tags);
	return Tags;
}
