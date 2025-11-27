#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "KRPlayerCameraManager.generated.h"

class FDebugDisplayInfo;
class UCanvas;

#define KORA_CAMERA_DEFAULT_FOV			(80.0f)
#define KORA_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define KORA_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

UCLASS()
class KORAPROJECT_API AKRPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AKRPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	virtual void DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	// UI 카메라 필요시 추후 추가
};
