#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotifyState_TimedCombatEffect.generated.h"

class UFXSystemComponent;

UCLASS(DisplayName = "KR Timed Combat Effect")
class KORAPROJECT_API UKRAnimNotifyState_TimedCombatEffect : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UKRAnimNotifyState_TimedCombatEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect") FGameplayTag EffectTag;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

#if WITH_EDITOR
	virtual bool CanBePlaced(UAnimSequenceBase* Animation) const override { return true; }
#endif

private:
	UPROPERTY()
	TWeakObjectPtr<UFXSystemComponent> SpawnedEffect;
};
