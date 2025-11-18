#include "Inventory/KRInventoryItemInstance.h"
#include "KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventoryItemInstance)

UKRInventoryItemInstance::UKRInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<FGameplayTag> UKRInventoryItemInstance::GetAllFragmentTags() const
{
	if (ItemDef)
	{
		return ItemDef->GetAllFragmentTags();
	}

	return TArray<FGameplayTag>();
}

void UKRInventoryItemInstance::SetItemDef(UKRInventoryItemDefinition* InDef)
{
	ItemDef = InDef;
}

UKRInventoryItemInstance* UKRInventoryItemInstance::CreateItemDefinition()
{
	ItemDef = UKRInventoryItemDefinition::CreateItemDefinition();
	
	return this;
}

const UKRInventoryItemFragment* UKRInventoryItemInstance::FindFragmentByTag(FGameplayTag Tag) const
{
	const UKRInventoryItemFragment* Fragment = nullptr;
	if (Tag.IsValid())
	{
		if (ItemDef)
		{
			Fragment = ItemDef->FindFragmentByTag(Tag);
		}	
	}
	
	return Fragment;
}
