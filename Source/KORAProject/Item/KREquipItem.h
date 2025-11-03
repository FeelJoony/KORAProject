#pragma once

#include "CoreMinimal.h"
#include "Item/KRBaseItem.h"
#include "KREquipItem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class KORAPROJECT_API UKREquipItem : public UKRBaseItem
{
	GENERATED_BODY()

public:
	virtual void Equip(AActor* InInstigator);
	virtual void Unequip(AActor* InInstigator);
};