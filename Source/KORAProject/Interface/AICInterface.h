#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AICInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAICInterface : public UInterface
{
	GENERATED_BODY()
};

class KORAPROJECT_API IAICInterface
{
	GENERATED_BODY()

public:
	virtual void Patrol() const = 0;
	virtual void Alert() const = 0;
	virtual void ChasePlayer() const = 0;
	virtual void SenseRange() const = 0;
};
