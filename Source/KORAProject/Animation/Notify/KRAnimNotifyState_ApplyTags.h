#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "KRAnimNotifyState_ApplyTags.generated.h"

UCLASS()
class KORAPROJECT_API UKRAnimNotifyState_ApplyTags : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
						   float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						  const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<struct FGameplayTag> GameplayTags;
};
