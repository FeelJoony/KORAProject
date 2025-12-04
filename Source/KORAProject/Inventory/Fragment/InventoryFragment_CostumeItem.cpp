#include "InventoryFragment_CostumeItem.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Inventory/KRInventoryItemInstance.h"

void UInventoryFragment_CostumeItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	if (CostumeID < 0) return;

	auto DataTables = Instance->GetWorld()->GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataTables) return;

	//FMaterialItemData* Row = DataTables->GetData<FMaterialItemData>(EGameDataType::MaterialData, MaterialID);
	//if (!Row) return;

	// Material 값 매핑
}