#include "Item/Weapons/KRWeaponBase.h"
#include "Weapons/KRWeaponInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Weapons/KRWeaponInstance.h"

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
}


void AKRWeaponBase::SetWeaponInstance(UKRWeaponInstance* InInstance)
{
	WeaponInstance = InInstance;
	OnWeaponInstanceAssigned();
}


void AKRWeaponBase::PlayUnequipEffect_Implementation()
{
	SetWeaponVisibility(false);
}

void AKRWeaponBase::PlayEquipEffect_Implementation()
{
	SetWeaponVisibility(true);
}
void AKRWeaponBase::OnWeaponInstanceAssigned()
{
	// 기본은 아무 것도 안함.
	// AKRRangeWeapon / AKRMeleeWeapon 같은 파생 클래스에서
	// Instance 기반으로 VFX, 스켈레탈 머티리얼, 스킨 변경 등 하고 싶으면 override 해서 사용.
}