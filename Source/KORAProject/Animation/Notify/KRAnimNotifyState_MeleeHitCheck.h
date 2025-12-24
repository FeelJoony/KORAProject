#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "KRAnimNotifyState_MeleeHitCheck.generated.h"

class UKRGameplayAbility_MeleeAttack;

/**
 * Melee Hit Check AnimNotifyState
 *
 * 근접 공격 GA의 Shape Trace 히트 체크를 트리거하는 AnimNotifyState
 *
 * 사용법:
 * - 공격 애니메이션의 히트 판정 구간에 배치
 * - NotifyBegin: 히트 체크 시작 (HitActors 초기화)
 * - NotifyTick: 매 프레임 Shape Trace 수행
 * - NotifyEnd: 히트 체크 종료
 */
UCLASS(DisplayName = "KR Melee Hit Check", meta = (DisplayName = "KR Melee Hit Check"))
class KORAPROJECT_API UKRAnimNotifyState_MeleeHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UKRAnimNotifyState_MeleeHitCheck();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
							float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						   float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						  const FAnimNotifyEventReference& EventReference) override;

protected:
	/** 현재 활성화된 MeleeAttack GA 조회 */
	UKRGameplayAbility_MeleeAttack* GetActiveMeleeAttackAbility(AActor* OwnerActor) const;
};
