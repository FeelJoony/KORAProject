#include "Item/Weapons/KRWeaponBase.h"
#include "Weapons/KRWeaponInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"

AKRWeaponBase::AKRWeaponBase()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKRWeaponBase::SetWeaponVisibility(bool bVisible)
{
	SetActorHiddenInGame(!bVisible);

	if (WeaponMesh)
	{
		WeaponMesh->SetVisibility(bVisible, true);
	}
	SetActorEnableCollision(bVisible);
	SetActorTickEnabled(bVisible);
}

void AKRWeaponBase::PlayUnequipEffect_Implementation()
{
	SetWeaponVisibility(false);
}

void AKRWeaponBase::PlayEquipEffect_Implementation()
{
	SetWeaponVisibility(true);
}
