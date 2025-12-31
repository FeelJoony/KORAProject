#include "Camera/KRCameraMode_StarDash.h"
#include "Components/KRCameraComponent.h"
#include "GameFramework/Character.h"

UKRCameraMode_StarDash::UKRCameraMode_StarDash()
{
	HeightOffset = 500.0f;
	BackwardDistance = 800.0f;
	LookAtHeightOffset = 100.0f;
	CameraInterpSpeed = 8.0f;
	bUsePlayerBackward = true;
	bFixedCameraPosition = true;
	
	SkillFOV = 90.0f;
	FieldOfView = SkillFOV;
	FOVInterpSpeed = 5.0f;
	
	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
	BlendFunction = EKRCameraModeBlendFunction::Linear;

	ViewPitchMin = -70.0f;
	ViewPitchMax = 10.0f;

	bPreventPenetration = false;
}

void UKRCameraMode_StarDash::OnActivation()
{
	Super::OnActivation();
	
	if (UKRCameraComponent* CameraComp = GetKRCameraComponent())
	{
		CurrentCameraLocation = CameraComp->GetComponentLocation();
		CurrentFOV = FieldOfView;
	}
}

void UKRCameraMode_StarDash::OnDeactivation()
{
	Super::OnDeactivation();
	ClearSkillLocations();
}

void UKRCameraMode_StarDash::SetSkillLocations(FVector InTargetLocation, FVector InPlayerStartLocation)
{
	TargetLocation = InTargetLocation;
	PlayerStartLocation = InPlayerStartLocation;
	bSkillLocationSet = true;

	FixedCameraLocation = CalculateTargetCameraLocation();
	CurrentCameraLocation = FixedCameraLocation;

	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
}

void UKRCameraMode_StarDash::ClearSkillLocations()
{
	bSkillLocationSet = false;
	TargetLocation = FVector::ZeroVector;
	PlayerStartLocation = FVector::ZeroVector;
	FixedCameraLocation = FVector::ZeroVector;
	PlayerActor.Reset();
}

void UKRCameraMode_StarDash::UpdateView(float DeltaTime)
{
	if (!bSkillLocationSet)
	{
		UE_LOG(LogTemp, Verbose, TEXT("CameraMode_StarDash: bSkillLocationSet is FALSE, using parent UpdateView"));
		Super::UpdateView(DeltaTime);
		return;
	}

	FVector FinalCameraLocation = FixedCameraLocation;
	CurrentFOV = FMath::FInterpTo(
		CurrentFOV,
		SkillFOV,
		DeltaTime,
		FOVInterpSpeed
	);
	
	FVector LookAtLocation = CalculateLookAtLocation();
	FVector LookDirection = (LookAtLocation - FinalCameraLocation).GetSafeNormal();
	FRotator LookRotation = LookDirection.Rotation();

	View.Location = FinalCameraLocation;
	View.Rotation = LookRotation;
	View.ControlRotation = LookRotation;
	View.FieldOfView = CurrentFOV;
}

FVector UKRCameraMode_StarDash::GetPivotLocation() const
{
	if (bSkillLocationSet)
	{
		return TargetLocation;
	}

	return Super::GetPivotLocation();
}

FRotator UKRCameraMode_StarDash::GetPivotRotation() const
{
	if (bSkillLocationSet)
	{
		FVector LookAtLocation = CalculateLookAtLocation();
		FVector CameraLocation = bFixedCameraPosition ? FixedCameraLocation : CalculateTargetCameraLocation();
		FVector LookDirection = (LookAtLocation - CameraLocation).GetSafeNormal();
		return LookDirection.Rotation();
	}

	return Super::GetPivotRotation();
}

FVector UKRCameraMode_StarDash::CalculateTargetCameraLocation() const
{
	if (!bSkillLocationSet)
	{
		return FVector::ZeroVector;
	}
	
	FVector ToTarget = (TargetLocation - PlayerStartLocation).GetSafeNormal2D();
	FVector BackwardDirection = -ToTarget;
	
	FVector CameraLocation = TargetLocation
		+ (BackwardDirection * BackwardDistance)
		+ FVector(0.0f, 0.0f, HeightOffset);

	return CameraLocation;
}

FVector UKRCameraMode_StarDash::CalculateLookAtLocation() const
{
	if (!bSkillLocationSet)
	{
		return FVector::ZeroVector;
	}
	if (PlayerActor.IsValid())
	{
		return PlayerActor->GetActorLocation() + FVector(0.0f, 0.0f, LookAtHeightOffset);
	}
	return TargetLocation + FVector(0.0f, 0.0f, LookAtHeightOffset);
}
