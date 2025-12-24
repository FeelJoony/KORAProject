#pragma once

#include "CoreMinimal.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "KRMeleeWeapon.generated.h"

/**
 * 근접 무기 액터
 *
 * Shape Trace 기반 히트 감지로 전환됨에 따라 BoxCollision 제거
 * 히트 감지는 UKRGameplayAbility_MeleeAttack + UKRAnimNotifyState_MeleeHitCheck에서 처리
 */
UCLASS()
class KORAPROJECT_API AKRMeleeWeapon : public AKRWeaponBase
{
	GENERATED_BODY()

public:
	AKRMeleeWeapon();

	virtual void ConfigureWeapon(UKRInventoryItemInstance* InInstance) override;
};
