#include "Inventory/Fragment/InventoryFragment_DisplayUI.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Data/ItemDataStruct.h"

void UInventoryFragment_DisplayUI::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);
	
	if (!Instance) { return; }
	
	const FGameplayTag ItemTag = Instance->GetItemTag();
	if (!ItemTag.IsValid()) { return; }
	
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) { return; }
	
	UGameInstance* GI = Cast<UGameInstance>(ContextObj);
	if (!GI) { return; }
	
	auto* DT = UKRDataTablesSubsystem::GetSafe(this);
	if (!DT) { return; }

	FItemDataStruct* Row = DT->GetData<FItemDataStruct>(ItemTag);
	if (!Row) { return; }

	DisplayNameKey  = Row->DisplayNameKey;
	DescriptionKey  = Row->DescriptionKey;
	ItemIcon        = Row->Icon;
	Price			= FMath::RoundToInt(Row->BasePrice);

	WeaponLevel = 5;
}