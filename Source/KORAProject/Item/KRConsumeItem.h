#pragma once

#include "CoreMinimal.h"
#include "Item/KRBaseItem.h"
#include "KRConsumeItem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class KORAPROJECT_API UKRConsumeItem : public UKRBaseItem
{
	GENERATED_BODY()

public:
	virtual void Use(AActor* InInstigator);
};