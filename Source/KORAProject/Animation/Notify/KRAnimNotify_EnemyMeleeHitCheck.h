#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "KRAnimNotify_EnemyMeleeHitCheck.generated.h"

UCLASS()
class KORAPROJECT_API UKRAnimNotify_EnemyMeleeHitCheck : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
