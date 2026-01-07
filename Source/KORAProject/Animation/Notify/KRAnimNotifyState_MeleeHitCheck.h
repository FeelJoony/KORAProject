#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotifyState_MeleeHitCheck.generated.h"

class UAbilitySystemComponent;

/**
 * Melee Hit Check AnimNotifyState (GameplayEvent 기반)
 *
 * 근접 공격 GA에 GameplayEvent를 발송하여 히트 체크를 트리거하는 AnimNotifyState
 * 특정 GA 클래스에 의존하지 않고 느슨한 결합 유지
 *
 * 사용법:
 * - 공격 애니메이션의 히트 판정 구간에 배치
 * - NotifyBegin: HitCheck.Begin 이벤트 발송
 * - NotifyTick: HitCheck.Tick 이벤트 발송 (매 프레임)
 * - NotifyEnd: HitCheck.End 이벤트 발송
 * - GA에서 WaitGameplayEvent로 이벤트 수신하여 처리
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
	/** 히트 체크 시작 이벤트 태그 */
	UPROPERTY(EditAnywhere, Category = "HitCheck", meta = (Categories = "Event"))
	FGameplayTag BeginEventTag;

	/** 히트 체크 틱 이벤트 태그 */
	UPROPERTY(EditAnywhere, Category = "HitCheck", meta = (Categories = "Event"))
	FGameplayTag TickEventTag;

	/** 히트 체크 종료 이벤트 태그 */
	UPROPERTY(EditAnywhere, Category = "HitCheck", meta = (Categories = "Event"))
	FGameplayTag EndEventTag;

private:
	/** GameplayEvent 발송 */
	void SendHitCheckEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& EventTag);

	/** ASC 가져오기 */
	UAbilitySystemComponent* GetASC(USkeletalMeshComponent* MeshComp) const;
};
