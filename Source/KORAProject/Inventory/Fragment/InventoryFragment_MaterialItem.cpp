#include "Inventory/Fragment/InventoryFragment_MaterialItem.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Inventory/KRInventoryItemInstance.h"
//#include "Data/MaterialItemData.h"

void UInventoryFragment_MaterialItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	if (MaterialID < 0) return;

	//FMaterialItemData* Row = DataTables->GetData<FMaterialItemData>(EGameDataType::MaterialData, MaterialID);
	//if (!Row) return;

	// Material 값 매핑
}