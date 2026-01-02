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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CombatEvent")
	EKRCombatEventType EventType = EKRCombatEventType::PlaySound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CombatEvent")
	FGameplayTag EventTag;
};
