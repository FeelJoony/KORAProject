#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "KRAnimNotify_Footstep.generated.h"

UCLASS()
class KORAPROJECT_API UKRAnimNotify_Footstep : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual FString GetNotifyName_Implementation() const override;
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footstep")
	FName FootstepType = "Walk";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footstep")
	FName FootSocketName = "foot_l";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footstep", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float VolumeMultiplier = 1.0f;

private:
	FGameplayTag GetSoundTagForSurface(UPhysicalMaterial* PhysMat, const FName& Type) const;
	FGameplayTag GetEffectTagForSurface(UPhysicalMaterial* PhysMat, const FName& Type) const;
	UPhysicalMaterial* DetectGroundSurface(USkeletalMeshComponent* MeshComp, const FName& SocketName) const;
};
