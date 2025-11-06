#include "Item/Object/KREquipItem.h"

UKREquipItem::UKREquipItem()
{
}

void UKREquipItem::Equip()
{
	if (!bIsEquipped)
	{
		bIsEquipped = true;
		UE_LOG(LogTemp, Log, TEXT("아이템 장착됨: %s"), *GetItemID().ToString());
	}
}

void UKREquipItem::Unequip()
{
	if (bIsEquipped)
	{
		bIsEquipped = false;
		UE_LOG(LogTemp, Log, TEXT("아이템 장착 해제: %s"), *GetItemID().ToString());
	}
}
