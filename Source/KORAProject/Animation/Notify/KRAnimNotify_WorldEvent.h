#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotify_WorldEvent.generated.h"


UCLASS()
class KORAPROJECT_API UKRAnimNotify_WorldEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual FString GetNotifyName_Implementation() const override;
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	FGameplayTag WorldEventTag;
};
