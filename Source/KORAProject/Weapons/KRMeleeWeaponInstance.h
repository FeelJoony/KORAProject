#pragma once

#include "CoreMinimal.h"
#include "Weapons/KRWeaponInstance.h"
#include "KRMeleeWeaponInstance.generated.h"

/**
 * 근접 무기 전용 WeaponInstance
 * - 근접 무기 전용 스탯, 콤보, 충돌 관련 확장 가능 필요 시 구현하겠습니다.
 */
UCLASS()
class KORAPROJECT_API UKRMeleeWeaponInstance : public UKRWeaponInstance
{
    GENERATED_BODY()

public:
    UKRMeleeWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // 필요 시 근접 전용 기능 확장
};
