#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotify_EndSection.generated.h"

class UAbilitySystemComponent;

UCLASS(DisplayName = "KR End Section", meta = (DisplayName = "KR End Section"))
class KORAPROJECT_API UKRAnimNotify_EndSection : public UAnimNotify
{
	GENERATED_BODY()

public:
	UKRAnimNotify_EndSection();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual FString GetNotifyName_Implementation() const override;
#endif

protected:
	UPROPERTY(EditAnywhere, Category = "EndSection", meta = (Categories = "Event"))
	FGameplayTag EndSectionEventTag;

private:
	void SendEndSectionEvent(USkeletalMeshComponent* MeshComp);
	UAbilitySystemComponent* GetASC(USkeletalMeshComponent* MeshComp) const;
};
