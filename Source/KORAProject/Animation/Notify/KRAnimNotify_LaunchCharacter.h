#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "KRAnimNotify_LaunchCharacter.generated.h"

UCLASS()
class KORAPROJECT_API UKRAnimNotify_LaunchCharacter : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Launch)
	FVector LaunchDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Launch)
	bool XYOverride = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Launch)
	bool ZOverride = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Launch)
	float LaunchSpeed;
};
