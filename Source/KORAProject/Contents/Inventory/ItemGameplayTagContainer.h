
#pragma once

#include "GameplayTagContainer.h"
#include "ItemGameplayTagContainer.generated.h"

USTRUCT(BlueprintType, meta = (HasNativeMake = "/Script/GameplayTags.BlueprintGameplayTagLibrary.MakeGameplayTagContainerFromArray", HasNativeBreak = "/Script/GameplayTags.BlueprintGameplayTagLibrary.BreakGameplayTagContainer"))
struct FItemGameplayTagContainer : public FGameplayTagContainer
{
	GENERATED_USTRUCT_BODY()
};
