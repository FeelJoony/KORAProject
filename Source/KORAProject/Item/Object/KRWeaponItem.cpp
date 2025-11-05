#include "Item/Object/KRWeaponItem.h"
#include "Data/WeaponItemData.h"
#include "Engine/Engine.h"

UKRWeaponItem::UKRWeaponItem()
{
	CurrentATK = 0;
}

void UKRWeaponItem::SetupWeaponStats()
{
	RefreshStats();
}

bool UKRWeaponItem::TryEnforce()
{
	EnforceLevel++;
	RefreshStats();
	return true;
}

void UKRWeaponItem::RefreshStats()
{
	const FWeaponItemData* Data = GetWeaponData();
	if (!Data) return;

	CurrentATK = Data->BaseATK + EnforceLevel * 2;
	CurrentCritChance = Data->BaseCritChance;
	CurrentCritMulti = Data->BaseCritMulti;
	CurrentRange = Data->BaseRange;
	CurrentAccuracy = Data->BaseAccuracy;
	CurrentRecoil = Data->BaseRecoil;
	CurrentCapacity = Data->BaseCapacity;
	CurrentAttackSpeed = Data->AttackSpeed;
	CurrentReloadTime = Data->ReloadTime;
}

const FWeaponItemData* UKRWeaponItem::GetWeaponData() const
{
	return nullptr;
}

