#include "Inventory/KRInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

//#include UE_INLINE_GENERATED_CPP_BY_NAME(KRInventoryItemDefinition)
//
//UKRInventoryItemDefinition::UKRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//}
//
//const UKRInventoryItemFragment* UKRInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UKRInventoryItemFragment> FragmentClass) const
//{
//	if (FragmentClass != nullptr)
//	{
//		for (UKRInventoryItemFragment* Fragment : Fragments)
//		{
//			if (Fragment && Fragment->IsA(FragmentClass))
//			{
//				return Fragment;
//			}
//		}
//	}
//
//	return nullptr;
//}
//
//const UKRInventoryItemFragment* UKRInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UKRInventoryItemDefinition> ItemDef, TSubclassOf<UKRInventoryItemFragment> FragmentClass)
//{
//	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
//	{
//		return GetDefault<UKRInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
//	}
//	return nullptr;
//}