#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_RangeBase.h"
#include "KRGameplayAbility_RangeAttack.generated.h"

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_RangeAttack : public UKRGameplayAbility_RangeBase
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_RangeAttack(const FObjectInitializer& ObjectInitializer);
};