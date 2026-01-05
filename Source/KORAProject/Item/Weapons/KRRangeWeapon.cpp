#include "Item/Weapons/KRRangeWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Equipment/KREquipmentInstance.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"

AKRRangeWeapon::AKRRangeWeapon()
{
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(GetRootComponent());
}

void AKRRangeWeapon::ConfigureWeapon(UKRInventoryItemInstance* InInstance)
{
	Super::ConfigureWeapon(InInstance);

	
	
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

void AKRRangeWeapon::FireProjectile(const FRotator& InOverrideRotation)
{
	if (!ProjectileClass || !GetWorld()) return;

	FVector SpawnLocation = MuzzlePoint->GetComponentLocation();
    
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();

	AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, InOverrideRotation, SpawnParams);

	if (Projectile)
	{
		if (UProjectileMovementComponent* MoveComp = Projectile->FindComponentByClass<UProjectileMovementComponent>())
		{
			MoveComp->Velocity = InOverrideRotation.Vector() * ProjectileSpeed;
		}
	}
}