#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Equipment/KREquipmentInstance.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Data/ItemDataStruct.h"
#include "Data/EquipDataStruct.h"
#include "Equipment/KREquipmentDefinition.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void UInventoryFragment_EquippableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (!Instance) { return; }

	LoadFromDataTable(Instance);
}

void UInventoryFragment_EquippableItem::LoadFromDataTable(UKRInventoryItemInstance* Instance)
{
	APlayerState* PS = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	if (PS)
	{
		APawn* Pawn = PS->GetPawn();
		if (Pawn)
		{
			UKREquipmentManagerComponent* EquipComp = Pawn->FindComponentByClass<UKREquipmentManagerComponent>();
			if (EquipComp)
			{
				EquipComp->AddEquip(Instance);
			}
		}
	}
	
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) { return; }

	UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(this);
	
	const FGameplayTag ItemTag = Instance->GetItemTag();
	FItemDataStruct* ItemRow = DataTablesSubsystem.GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
	if (!ItemRow) { return; }
	
	FEquipDataStruct* EquipDataStruct = DataTablesSubsystem.GetData<FEquipDataStruct>(EGameDataType::EquipData, ItemRow->EquipID);
	
	EquipmentDefinition = NewObject<UKREquipmentDefinition>(this);
}
