#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Equipment/KREquipmentInstance.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/EquipDataStruct.h"

void UInventoryFragment_EquippableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);
	
	InitializeEquipInstance(Instance);
}

void UInventoryFragment_EquippableItem::InitializeEquipInstance(UKRInventoryItemInstance* Instance)
{
	if (!Instance) return;
	if (EquipInstance && EquipInstance->IsValid()) return;

	UKRDataTablesSubsystem* DataTablesSubsystem = UKRDataTablesSubsystem::GetSafe(this);
	if (!DataTablesSubsystem)
	{
		return;
	}

	const FEquipDataStruct* EquipData = DataTablesSubsystem->GetEquipDataByItemTag(Instance->GetItemTag());
	if (!EquipData)
	{
		return;
	}
	EquipInstance = NewObject<UKREquipmentInstance>(Instance);
	EquipInstance->InitializeFromData(EquipData);
}
