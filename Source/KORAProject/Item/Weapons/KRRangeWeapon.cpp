#include "Item/Weapons/KRRangeWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AKRRangeWeapon::AKRRangeWeapon()
{
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(GetRootComponent());
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
