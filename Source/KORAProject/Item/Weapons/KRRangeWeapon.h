#pragma once

#include "Item/Weapons/KRWeaponBase.h"
#include "KRRangeWeapon.generated.h"

class AActor;
class USceneComponent;

/**
 * 원거리 무기 Actor
 * - 총구 위치에서 Projectile 또는 트레이스 발사
 */
UCLASS()
class KORAPROJECT_API AKRRangeWeapon : public AKRWeaponBase
{
	GENERATED_BODY()

public:
	AKRRangeWeapon();

	/** 발사 함수 (Projectile 또는 트레이스 기반) */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Range")
	virtual void FireProjectile();

	virtual FTransform GetMuzzleTransform() const override;
	
protected:
	/** 발사체 스폰 위치 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Range")
	TObjectPtr<USceneComponent> MuzzlePoint;

	/** 발사할 Projectile 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Range")
	TSubclassOf<AActor> ProjectileClass;

	/** 발사체 속도 (없으면 Default 사용) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Range")
	float ProjectileSpeed = 2000.f;
};
