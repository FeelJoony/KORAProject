#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotifyState_EnemySlash.generated.h"

class UAbilitySystemComponent;

/**
 * Enemy Slash Hit Check AnimNotifyState (GameplayEvent 기반)
 *
 * 적 슬래시 공격의 히트 체크를 트리거하는 AnimNotifyState
 * 특정 캐릭터/컨트롤러 클래스에 의존하지 않고 GameplayEvent를 통해 통신
 *
 * 사용법:
 * - 적 공격 애니메이션의 히트 판정 구간에 배치
 * - NotifyBegin: HitCheck 이벤트 발송
 * - GA에서 WaitGameplayEvent로 이벤트 수신하여 처리
 */
UCLASS(DisplayName = "KR Enemy Slash Hit Check")
class KORAPROJECT_API UKRAnimNotifyState_EnemySlash : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UKRAnimNotifyState_EnemySlash();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

protected:
	/** 히트 체크 이벤트 태그 */
	UPROPERTY(EditAnywhere, Category = "HitCheck", meta = (Categories = "Event"))
	FGameplayTag HitCheckEventTag;

private:
	/** GameplayEvent 발송 */
	void SendHitCheckEvent(USkeletalMeshComponent* MeshComp);

	/** ASC 가져오기 */
	UAbilitySystemComponent* GetASC(USkeletalMeshComponent* MeshComp) const;
};
