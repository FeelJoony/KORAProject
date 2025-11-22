#include "Inventory/Fragment/InventoryFragment_ModuleItem.h"

void UInventoryFragment_ModuleItem::ApplyToStats(class UInventoryFragment_SetStats* Stats)
{
	if (!Stats) return;

	// for (const FModuleStatEntry& Entry : ModuleStats)
	// {
	// 	Stats->AddStat(Entry.StatTag, Entry.Value);
	// }
}

void UInventoryFragment_ModuleItem::RemoveFromStats(class UInventoryFragment_SetStats* Stats)
{
	if (!Stats) return;

	// for (const FModuleStatEntry& Entry : ModuleStats)
	// {
	// 	Stats->SubtractStat(Entry.StatTag, Entry.Value);
	// }
}
