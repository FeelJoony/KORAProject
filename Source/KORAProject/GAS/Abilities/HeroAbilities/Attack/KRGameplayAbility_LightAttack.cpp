#include "KRGameplayAbility_LightAttack.h"
#include "Equipment/KREquipmentManagerComponent.h"

UKRGameplayAbility_LightAttack::UKRGameplayAbility_LightAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 기본 공격 설정 (에디터에서 조정 가능)
	DefaultAttackConfig.AttackShape = EKRAttackShapeType::Cone;
	DefaultAttackConfig.AttackRange = 200.0f;
	DefaultAttackConfig.ConeAngle = 90.0f;
	DefaultAttackConfig.DamageMultiplier = 1.0f;
	DefaultAttackConfig.HitIntensity = EKRHitIntensity::Light;
	DefaultAttackConfig.bUseHitStop = true;
	DefaultAttackConfig.HitStopDuration = 0.03f;
}

const TArray<TObjectPtr<UAnimMontage>>* UKRGameplayAbility_LightAttack::GetMontageArrayFromEntry(
	const FKRAppliedEquipmentEntry& Entry) const
{
	return &Entry.GetLightAttackMontages();
}
