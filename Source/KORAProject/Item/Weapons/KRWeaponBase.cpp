#include "Item/Weapons/KRWeaponBase.h"
#include "Data/DataAssets/DataAsset_Weapon.h"
#include "Data/DataAssets/DataAsset_HeroWeapon.h"

#include "KORADebugHelper.h"


AKRWeaponBase::AKRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponMesh->SetGenerateOverlapEvents(false);
}

void AKRWeaponBase::AssignGrantAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
	GrantedAbilitySpecHandles = InSpecHandles;
}

TArray<FGameplayAbilitySpecHandle> AKRWeaponBase::GetGrantedAbilitySpecHandles() const
{
	return GrantedAbilitySpecHandles;
}

FKRWeaponCommonData AKRWeaponBase::GetCommonData() const
{
	if (WeaponData)
	{
		return WeaponData->CommonData;
	}

	Debug::Print(TEXT("WeaponData is missing"), FColor::Red);
	return FKRWeaponCommonData();
}

bool AKRWeaponBase::GetHeroData(FKRHeroWeaponData& OutHeroData) const
{
	OutHeroData = FKRHeroWeaponData();
	
	if (!WeaponData) 
	{
		return false;
	}
	
	if (const UDataAsset_HeroWeapon* HeroDataAsset = Cast<const UDataAsset_HeroWeapon>(WeaponData))
	{
		OutHeroData = HeroDataAsset->HeroWeaponData;
		return true;
	}

	return false; 
}



void AKRWeaponBase::ReportHit(AActor* HitActor, const FHitResult& HitResult)
{
	OnWeaponHitTarget.Broadcast(HitActor, HitResult);
}

void AKRWeaponBase::ReportPulled(AActor* HitActor, const FHitResult& HitResult)
{
	OnWeaponPulledFromTarget.Broadcast(HitActor, HitResult);
}
