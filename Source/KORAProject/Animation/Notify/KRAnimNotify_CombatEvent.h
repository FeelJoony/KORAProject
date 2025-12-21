#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotify_CombatEvent.generated.h"

/**
 * 전투 이벤트 타입 - 간략화 버전
 * Sound, Effect 재생만 담당 (HitCheck, MotionWarp은 GA에서 처리)
 */
UENUM(BlueprintType)
enum class EKRCombatEventType : uint8
{
	PlaySound,
	SpawnEffect
};

/**
 * 전투 이벤트 AnimNotify - 간략화 버전
 *
 * 애니메이션에서 Sound/Effect 트리거만 담당
 * - HitCheck: HitCheck NotifyState 또는 GA에서 처리
 * - MotionWarping: UE5 내장 AnimNotifyState_MotionWarping 사용
 * - HitReaction: UKRGA_HitReaction GA에서 처리
 */
UCLASS()
class KORAPROJECT_API UKRAnimNotify_CombatEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual FString GetNotifyName_Implementation() const override;
#endif

protected:
	// 이벤트 타입 (Sound 또는 Effect)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CombatEvent")
	EKRCombatEventType EventType = EKRCombatEventType::PlaySound;

	// 이벤트 태그 (사운드/이펙트 태그)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CombatEvent")
	FGameplayTag EventTag;

	// 소켓 이름 (이펙트 스폰 위치)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CombatEvent", meta = (EditCondition = "EventType == EKRCombatEventType::SpawnEffect"))
	FName SocketName = NAME_None;
};
