#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "KRAIStateTreeTaskBase.generated.h"

UCLASS()
class KORAPROJECT_API UKRAIStateTreeTaskBase : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UKRAIStateTreeTaskBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
