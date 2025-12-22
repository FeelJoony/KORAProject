#include "Inventory/KRInventoryItemInstance.h"
#include "KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventoryItemInstance)

UKRInventoryItemInstance::UKRInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRInventoryItemInstance::SetItemDef(UKRInventoryItemDefinition* InDef)
{
	ItemDef = InDef;
	if (ItemDef)
	{
		DuplicateFragmentsFromDefinition();
	}
}

void UKRInventoryItemInstance::DuplicateFragmentsFromDefinition()
{
	if (!ItemDef)
	{
		return;
	}

	InstanceFragments.Empty();
	
	TArray<FGameplayTag> FragmentTags = ItemDef->GetAllFragmentTags();
	for (const FGameplayTag& Tag : FragmentTags)
	{
		if (const UKRInventoryItemFragment* SourceFragment = ItemDef->FindFragmentByTag(Tag))
		{
			UKRInventoryItemFragment* DuplicatedFragment = DuplicateObject<UKRInventoryItemFragment>(SourceFragment, this);

			if (DuplicatedFragment)
			{
				InstanceFragments.Add(Tag, DuplicatedFragment);
				DuplicatedFragment->OnInstanceCreated(this);
			}
		}
	}
}

void UKRInventoryItemInstance::AddInstanceFragment(FGameplayTag Tag, UKRInventoryItemFragment* Fragment)
{
	if (Tag.IsValid() && Fragment)
	{
		InstanceFragments.Add(Tag, Fragment);
	}
}

UKRInventoryItemInstance* UKRInventoryItemInstance::CreateItemDefinition()
{
	ItemDef = UKRInventoryItemDefinition::CreateItemDefinition();
	return this;
}

const UKRInventoryItemFragment* UKRInventoryItemInstance::FindFragmentByClass(TSubclassOf<UKRInventoryItemFragment> FragmentClass) const
{
	if (!FragmentClass)
	{
		return nullptr;
	}

	for (const auto& FragmentPair : InstanceFragments)
	{
		if (FragmentPair.Value && FragmentPair.Value->IsA(FragmentClass))
		{
			return FragmentPair.Value;
		}
	}
	
	if (ItemDef)
	{
		return ItemDef->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

UKRInventoryItemFragment* UKRInventoryItemInstance::FindInstanceFragmentByClass(TSubclassOf<UKRInventoryItemFragment> FragmentClass)
{
	if (!FragmentClass)
	{
		return nullptr;
	}
	
	for (auto& FragmentPair : InstanceFragments)
	{
		if (FragmentPair.Value && FragmentPair.Value->IsA(FragmentClass))
		{
			return FragmentPair.Value;
		}
	}

	return nullptr;
}

const UKRInventoryItemFragment* UKRInventoryItemInstance::FindFragmentByTag(FGameplayTag Tag) const
{
	if (!Tag.IsValid())
	{
		return nullptr;
	}
	
	if (const TObjectPtr<UKRInventoryItemFragment>* Found = InstanceFragments.Find(Tag))
	{
		return *Found;
	}
	
	if (ItemDef)
	{
		return ItemDef->FindFragmentByTag(Tag);
	}

	return nullptr;
}

TArray<FGameplayTag> UKRInventoryItemInstance::GetAllFragmentTags() const
{
	TArray<FGameplayTag> Tags;
	
	InstanceFragments.GetKeys(Tags);
	if (ItemDef)
	{
		TArray<FGameplayTag> DefTags = ItemDef->GetAllFragmentTags();
		for (const FGameplayTag& Tag : DefTags)
		{
			Tags.AddUnique(Tag);
		}
	}

	return Tags;
}
