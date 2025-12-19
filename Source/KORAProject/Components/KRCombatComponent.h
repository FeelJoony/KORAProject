#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "KRCombatComponent.generated.h"

class UKRAbilitySystemComponent;
class UKREquipmentManagerComponent;
class AKRWeaponBase;

UCLASS()
class KORAPROJECT_API UKRCombatComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UKRCombatComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	// [New] 현재 장착된 무기를 EquipmentManager를 통해 가져옴
	AKRWeaponBase* GetCurrentActiveWeapon() const;

	// 근접 공격 히트 처리 (MeleeWeapon에서 호출)
	void HandleMeleeHit(AActor* HitActor, const FHitResult& Hit);
	
	// 원거리 발사 (GA에서 호출 -> EquipmentManager의 RangeWeapon에게 전달)
	bool FireRangedWeapon();
    
	// [Refactoring] 데미지 적용 (GAS Event 전송)
	void ApplyDamageToTarget(AActor* TargetActor, float BaseDamage, bool bIsCritical, const FHitResult* HitResult);

protected:
	UPROPERTY()
	TObjectPtr<UKRAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UKREquipmentManagerComponent> EquipmentManager;
};
