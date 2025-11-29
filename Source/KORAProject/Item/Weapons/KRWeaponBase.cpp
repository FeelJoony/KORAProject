#include "Item/Weapons/KRWeaponBase.h"
#include "Components/SkeletalMeshComponent.h"

AKRWeaponBase::AKRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}
