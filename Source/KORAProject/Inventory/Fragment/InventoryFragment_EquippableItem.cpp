#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Equipment/KREquipmentInstance.h"
#include "Equipment/KREquipmentDefinition.h"

void UInventoryFragment_EquippableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (!Instance) { return; }

	// Fragment는 순수 데이터 컨테이너 역할만 수행
	// EquipmentManager가 EquipItem() 호출 시 EquipInstance를 생성함
	EquipmentDefinition = NewObject<UKREquipmentDefinition>(this);
}
