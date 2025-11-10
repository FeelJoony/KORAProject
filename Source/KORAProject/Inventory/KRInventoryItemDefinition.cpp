#include "Inventory/KRInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventoryItemDefinition)

UKRInventoryItemDefinition::UKRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UKRInventoryItemFragment* UKRInventoryItemDefinition::FindFragmentByTag(FGameplayTag Tag) const
{
	if (Tag.IsValid())
	{
		if (const auto FoundFragment = FragmentContainer.Find(Tag))
		{
			return GetDefault<UKRInventoryItemFragment>(*FoundFragment);
		}
	}
	return nullptr;
}

void UKRInventoryItemDefinition::AddFragment(FGameplayTag Tag, TSubclassOf<UKRInventoryItemFragment> Fragment)
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
