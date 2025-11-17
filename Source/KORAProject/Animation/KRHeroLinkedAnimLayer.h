#pragma once

#include "CoreMinimal.h"
#include "Animation/KRBaseAnimInstance.h"
#include "KRHeroLinkedAnimLayer.generated.h"

class UKRHeroAnimInstance;

UCLASS()
class KORAPROJECT_API UKRHeroLinkedAnimLayer : public UKRBaseAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UKRHeroAnimInstance* GetHeroAnimInstance() const;
};
