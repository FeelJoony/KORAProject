#include "Item/Weapons/KRWeaponBase.h"
#include "Components/StaticMeshComponent.h"


AKRWeaponBase::AKRWeaponBase()
{
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
	
	// 이펙트 처리 추가 가능
}

