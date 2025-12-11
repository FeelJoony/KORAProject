#include "Inventory/Fragment/InventoryFragment_SellableItem.h"
#include "GameplayTagContainer.h"
#include "Data/ItemDataStruct.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Inventory/KRInventoryItemInstance.h"

struct FItemDataStruct;

void UKRInventoryFragment_SellableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (!Instance) { return; }
	
	const FGameplayTag ItemTag = Instance->GetItemTag();
	if (!ItemTag.IsValid()) { return; }
	
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) { return; }
	
	UGameInstance* GI = Cast<UGameInstance>(ContextObj);
	if (!GI) { return; }
	
	auto* DT = GI->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DT) { return; }

	FItemDataStruct* Row = DT->GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
	if (!Row) { return; }

	BasePrice = Row->BasePrice;
}
