#pragma once

#include "CoreMinimal.h"
#include "KRCameraMode_ThirdPerson.h"
#include "KRCameraMode_LockOn.generated.h"

UCLASS()
class KORAPROJECT_API UKRCameraMode_LockOn : public UKRCameraMode_ThirdPerson
{
	GENERATED_BODY()

public:
	UKRCameraMode_LockOn();
};
