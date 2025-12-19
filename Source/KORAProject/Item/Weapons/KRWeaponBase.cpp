#include "Item/Weapons/KRWeaponBase.h"

#include "IDetailTreeNode.h"
#include "Weapons/KRWeaponInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/KRInventoryItemInstance.h"
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

void AKRWeaponBase::ConfigureWeapon(UKRInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
	if (ItemInstance)
	{
		const UInventoryFragment_EquippableItem* Equippable = ItemInstance->FindFragmentByClass<UInventoryFragment_EquippableItem>();
		if (Equippable)
		{
			if (const UKREquipmentInstance* EquipInst = Equippable->GetEquipInstance())
			{
				if (const UKREquipmentDefinition* Def = EquipInst->GetDefinition())
				{
					if (const FEquipDataStruct* Data = Def->GetEquipDataStruct())
					{
						if (Data->OverrideMaterials.IsValidIndex(0))
						{
							WeaponMesh->SetMaterial(0, Data->OverrideMaterials[0].LoadSynchronous());
						}
					}
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
