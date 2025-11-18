#include "InventoryFragment_SetStats.h"

void UInventoryFragment_SetStats::OnInstanceCreated(class UKRInventoryItemInstance* Instance)
{
	
}

void UInventoryFragment_SetStats::InitStat(FGameplayTag StatTag, double StatValue)
{
	if (!Stats.Contains(StatTag))
	{
		return;
	}

	Stats.Add(StatTag, StatValue);
}

void UInventoryFragment_SetStats::AddStat(FGameplayTag StatTag, double StatValue)
{
	if (!Stats.Contains(StatTag))
	{
		return;
	}

	Stats[StatTag] += StatValue;
}

void UInventoryFragment_SetStats::SubtractStat(FGameplayTag StatTag, double StatValue)
{
	if (!Stats.Contains(StatTag))
	{
		return;
	}

	double& ContainStat = Stats[StatTag];
	if (ContainStat < StatValue)
	{
		Stats[StatTag] = 0.0;
		
		return;
	}

	Stats[StatTag] -= StatValue;
}

void UInventoryFragment_SetStats::ClearStat(FGameplayTag StatTag)
{
	if (!Stats.Contains(StatTag))
	{
		return;
	}

	Stats.Remove(StatTag);
}

double UInventoryFragment_SetStats::GetStatByTag(FGameplayTag StatTag) const
{
	if (const double* StatPtr = Stats.Find(StatTag))
	{
		return *StatPtr;
	}

	return 0;
}
