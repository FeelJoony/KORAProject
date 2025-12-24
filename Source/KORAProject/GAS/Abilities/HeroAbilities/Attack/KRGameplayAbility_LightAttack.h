#pragma once

#include "CoreMinimal.h"
#include "KRGameplayAbility_MeleeAttack.h"
#include "KRGameplayAbility_LightAttack.generated.h"

/**
 * Light Attack GA (일반 공격)
 * - 새로운 Shape Trace 기반 히트 감지
 * - Motion Warping 지원
 * - 콤보 시스템
 */
UCLASS()
class KORAPROJECT_API UKRGameplayAbility_LightAttack : public UKRGameplayAbility_MeleeAttack
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_LightAttack(const FObjectInitializer& ObjectInitializer);

protected:
	/** LightAttack 몽타주 배열 반환 */
	virtual const TArray<TObjectPtr<UAnimMontage>>* GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const override;
};
