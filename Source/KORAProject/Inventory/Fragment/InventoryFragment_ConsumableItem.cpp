#include "Inventory/Fragment/InventoryFragment_ConsumableItem.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Subsystem/KRDataTablesSubsystem.h"
//#include "ConsumeData.h"

void UInventoryFragment_ConsumableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (!Instance || ConsumeID < 0)
	{
		return;
	}
	
	auto* DataTables = Instance->GetWorld()
		->GetGameInstance()
		->GetSubsystem<UKRDataTablesSubsystem>();

	if (!DataTables)
	{
		return;
	}

	// if (FConsumeData* Row = DataTables->GetData<FConsumeData>(EGameDataType::ConsumeData, ConsumeID))
	// {
	// 	AbilityTags = Row->AbilityTags;   // 여러 개 태그 복사
	// 	Power       = Row->Power;
	// 	Duration    = Row->Duration;
	// }
}
