#include "Camera/KRPlayerCameraManager.h"
#include "Components/KRCameraComponent.h"
#include "Engine/Canvas.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

AKRPlayerCameraManager::AKRPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = KORA_CAMERA_DEFAULT_FOV;
	ViewPitchMin = KORA_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = KORA_CAMERA_DEFAULT_PITCH_MAX;
}

void AKRPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	if (const UKRCameraComponent* CameraComponent = UKRCameraComponent::FindCameraComponent(OutVT.Target))
	{
		OutVT.POV.Location = CameraComponent->GetComponentLocation();
		OutVT.POV.Rotation = CameraComponent->GetComponentRotation();
		OutVT.POV.FOV = CameraComponent->FieldOfView;
		OutVT.POV.AspectRatio = CameraComponent->AspectRatio;
	}
	else
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
	}

}

void AKRPlayerCameraManager::DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL,
	float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("KRPlayerCameraManager: %s"), *GetNameSafe(this)));
	
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);
	if (const UKRCameraComponent* CameraComponent = UKRCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}
