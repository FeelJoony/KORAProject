#pragma once

#include "CoreMinimal.h"
#include "Weapons/KRWeaponInstance.h"
#include "KRRangeWeaponInstance.generated.h"

/**
 * 원거리 무기 전용 WeaponInstance
 * - 탄창, 장탄수, Reload 기능 확장
 * - 발사 로직은 WeaponActor(AKRRangeWeapon)에서 수행
 */
UCLASS()
class KORAPROJECT_API UKRRangeWeaponInstance : public UKRWeaponInstance
{
    GENERATED_BODY()

public:
    UKRRangeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /** 탄약 소모 */
    UFUNCTION(BlueprintCallable, Category = "Weapon|Range")
    bool ConsumeAmmo(int32 Amount = 1);

    /** 재장전 */
    UFUNCTION(BlueprintCallable, Category = "Weapon|Range")
    void Reload();

    UFUNCTION(BlueprintCallable, Category = "Weapon|Range")
    FORCEINLINE float GetSpreadAngleDegrees() const { return 0.4f; }
    
};
