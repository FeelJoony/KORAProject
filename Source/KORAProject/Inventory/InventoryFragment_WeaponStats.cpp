#include "Inventory/InventoryFragment_WeaponStats.h"

void UInventoryFragment_WeaponStats::IncreaseReforgeLevel()
{
	ReforgeLevel++;
}

void UInventoryFragment_WeaponStats::ApplyReforgeStats(const struct FWeaponReforgeRow& Row)
{
	// BaseATK += Row.AddATK;
	// CritChance += Row.AddCritChance;
	// Range += Row.AddRange;
}


void UInventoryFragment_WeaponStats::ApplyModifier(float AddCritMultiplier, float AddAttackSpeed, float AddAccuracy,
	float ReduceRecoil, int32 AddCapacity)
{
	CritMultiplier += AddCritMultiplier;
	AttackSpeed    += AddAttackSpeed;
	Accuracy       += AddAccuracy;
	Recoil         -= ReduceRecoil;
	Capacity       += AddCapacity;
}

void UInventoryFragment_WeaponStats::RemoveModifier(float AddCritMultiplier, float AddAttackSpeed, float AddAccuracy,
	float ReduceRecoil, int32 AddCapacity)
{
	CritMultiplier -= AddCritMultiplier;
	AttackSpeed    -= AddAttackSpeed;
	Accuracy       -= AddAccuracy;
	Recoil         += ReduceRecoil;
	Capacity       -= AddCapacity;
}
