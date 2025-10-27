#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TableKey.generated.h"

UINTERFACE(MinimalAPI)
class UTableKey : public UInterface
{
	GENERATED_BODY()
};

class KORAPROJECT_API ITableKey
{
	GENERATED_BODY()

public:
	virtual int32 GetKey() const = 0;
};
