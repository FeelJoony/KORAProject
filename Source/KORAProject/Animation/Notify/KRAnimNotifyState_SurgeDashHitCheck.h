#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotifyState_SurgeDashHitCheck.generated.h"

class UAbilitySystemComponent;

/**
 * 대시 히트 체크 NotifyState (GameplayEvent 기반)
 *
 * 몽타주의 대시 구간에 배치하여 지속적으로 히트 체크 이벤트를 발송합니다.
 * 특정 어빌리티 클래스에 의존하지 않고 GameplayEvent를 통해 통신합니다.
 *
 * 사용법:
 * 1. 대시 몽타주의 대시 구간에 이 NotifyState 배치
 * 2. 어빌리티에서 WaitGameplayEvent로 HitCheckEventTag 수신
 * 3. 이벤트 수신 시 히트 체크 로직 수행
 */
UCLASS(DisplayName = "KR Dash Hit Check (Event)")
class KORAPROJECT_API UKRAnimNotifyState_SurgeDashHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UKRAnimNotifyState_SurgeDashHitCheck();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override { return TEXT("Dash Hit Check"); }

protected:
	/** 발송할 히트 체크 이벤트 태그 */
	UPROPERTY(EditAnywhere, Category = "HitCheck", meta = (Categories = "Event"))
	FGameplayTag HitCheckEventTag;

	/** 히트 체크 간격 (초) */
	UPROPERTY(EditAnywhere, Category = "HitCheck", meta = (ClampMin = "0.01", ClampMax = "0.5"))
	float HitCheckInterval = 0.05f;

private:
	float TimeSinceLastCheck = 0.0f;

	/** GameplayEvent 발송 */
	void SendHitCheckEvent(USkeletalMeshComponent* MeshComp);

	/** ASC 가져오기 */
	UAbilitySystemComponent* GetASC(USkeletalMeshComponent* MeshComp) const;
};
