#include "Item/Weapons/KRWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"

AKRWeaponBase::AKRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

FTransform AKRWeaponBase::GetMuzzleTransform() const
{
	if (WeaponMesh && WeaponMesh->DoesSocketExist(TEXT("Muzzle")))
	{
		return WeaponMesh->GetSocketTransform(TEXT("Muzzle"));
	}
	return GetActorTransform();
}

