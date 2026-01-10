#include "Inventory/Fragment/InventoryFragment_QuestItem.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/ItemDataStruct.h"
#include "Data/GameDataType.h"

void UInventoryFragment_QuestItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	if (!Instance)
	{
		return;
	}

	FGameplayTag QuestItemTag = Instance->GetItemTag();
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) return;

	UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(ContextObj);
	const FItemDataStruct* ItemData = DataTablesSubsystem.GetData<FItemDataStruct>(QuestItemTag);
	if (ItemData)
	{
		QuestID = ItemData->QuestID;
	}
}