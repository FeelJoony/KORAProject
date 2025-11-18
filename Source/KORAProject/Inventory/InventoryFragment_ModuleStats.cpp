// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryFragment_ModuleStats.h"

void UInventoryFragment_ModuleStats::ApplyToWeapon(class UInventoryFragment_WeaponStats* WeaponStats)
{
	if (!WeaponStats) return;

	WeaponStats->ApplyModifier(
		Modifier.AddCritMultiplier,
		Modifier.AddAttackSpeed,
		Modifier.AddAccuracy,
		Modifier.RecoilReduce,
		Modifier.AddCapacity
	);
}

void UInventoryFragment_ModuleStats::RemoveFromWeapon(class UInventoryFragment_WeaponStats* WeaponStats)
{
	if (!WeaponStats) return;

	WeaponStats->RemoveModifier(
		Modifier.AddCritMultiplier,
		Modifier.AddAttackSpeed,
		Modifier.AddAccuracy,
		Modifier.RecoilReduce,
		Modifier.AddCapacity
	);
}
