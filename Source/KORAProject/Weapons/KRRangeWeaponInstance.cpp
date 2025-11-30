#include "Weapons/KRRangeWeaponInstance.h"

UKRRangeWeaponInstance::UKRRangeWeaponInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UKRRangeWeaponInstance::ConsumeAmmo(int32 Amount)
{
    if (CurrentAmmo < Amount)
    {
        return false; // 발사 불가
    }

    CurrentAmmo -= Amount;
    return true;
}

void UKRRangeWeaponInstance::Reload()
{
    CurrentAmmo = MaxAmmo;
}
