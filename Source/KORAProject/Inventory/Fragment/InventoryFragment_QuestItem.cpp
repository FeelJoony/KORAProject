#include "Inventory/Fragment/InventoryFragment_QuestItem.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Inventory/KRInventoryItemInstance.h"
//#include "Data/QuestItemData.h"

void UInventoryFragment_QuestItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	if (QuestID < 0) return;

	auto DataTables = Instance->GetWorld()->GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataTables) return;

	//FQuestItemData* Row = DataTables->GetData<FQuestItemData>(EGameDataType:QuestData, QuestID);
	//if (!Row) return;

	// Quest 값 매핑
}