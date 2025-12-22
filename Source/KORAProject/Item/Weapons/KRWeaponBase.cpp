#include "Item/Weapons/KRWeaponBase.h"

#include "Components/StaticMeshComponent.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Equipment/KREquipmentInstance.h"

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

void AKRWeaponBase::ConfigureWeapon(UKRInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
	if (!ItemInstance)
	{
		return;
	}

	const UInventoryFragment_EquippableItem* Equippable = ItemInstance->FindFragmentByClass<UInventoryFragment_EquippableItem>();
	if (!Equippable)
	{
		return;
	}

	const UKREquipmentInstance* EquipInst = Equippable->GetEquipInstance();
	if (!EquipInst || !EquipInst->IsValid())
	{
		return;
	}

	const FEquipDataStruct& Data = EquipInst->GetEquipData();
	if (!Data.EquipmentMesh.IsNull())
	{
		UStaticMesh* LoadedMesh = Data.EquipmentMesh.LoadSynchronous();
		if (LoadedMesh && WeaponMesh)
		{
			WeaponMesh->SetStaticMesh(LoadedMesh);
		}
	}
	if (WeaponMesh)
	{
		for (int32 i = 0; i < Data.OverrideMaterials.Num(); ++i)
		{
			if (!Data.OverrideMaterials[i].IsNull())
			{
				UMaterialInterface* LoadedMaterial = Data.OverrideMaterials[i].LoadSynchronous();
				if (LoadedMaterial)
				{
					WeaponMesh->SetMaterial(i, LoadedMaterial);
				}
			}
		}
	}
}

FGameplayTag AKRWeaponBase::GetWeaponItemTag() const
{
	return ItemInstance ? ItemInstance->GetItemTag() : FGameplayTag();
}

void AKRWeaponBase::PlayUnequipEffect_Implementation()
{
	SetWeaponVisibility(false);
}

void AKRWeaponBase::PlayEquipEffect_Implementation()
{
	SetWeaponVisibility(true);
}
