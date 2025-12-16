#include "Item/Weapons/KRRangeWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Equipment/KREquipmentInstance.h"
#include "Equipment/KREquipmentDefinition.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"

AKRRangeWeapon::AKRRangeWeapon()
{
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(GetRootComponent());
}

void AKRRangeWeapon::ConfigureWeapon(const UInventoryFragment_EquippableItem* Equippable, const UInventoryFragment_SetStats* SetStat)
{
	if (UKREquipmentInstance* EquipInstance = EquippableFragment->GetEquipInstance())
	{
		if (const UKREquipmentDefinition* EquipDefinition = EquipInstance->GetDefinition())
		{
			if (const FEquipDataStruct* EquipDataStruct = EquipDefinition->GetEquipDataStruct())
			{
				WeaponMesh->SetMaterial(0, EquipDataStruct->OverrideMaterials[0].LoadSynchronous());
			}
		}
	}

	// ToDo : Stat 적용 로직 구현
}

void AKRRangeWeapon::FireProjectile()
{
	if (!ProjectileClass || !GetWorld()) return;

	const FTransform MuzzleTM = GetMuzzleTransform();
	FVector SpawnLocation = MuzzlePoint->GetComponentLocation();
	FRotator SpawnRotation = MuzzlePoint->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();

	AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		// 속도 적용
		if (UProjectileMovementComponent* MoveComp = Projectile->FindComponentByClass<UProjectileMovementComponent>())
		{
			MoveComp->Velocity = SpawnRotation.Vector() * ProjectileSpeed;
		}
	}
}

FTransform AKRRangeWeapon::GetMuzzleTransform() const
{
	if (MuzzlePoint)
	{
		return MuzzlePoint->GetComponentTransform();
	}
	return FTransform::Identity;
}
