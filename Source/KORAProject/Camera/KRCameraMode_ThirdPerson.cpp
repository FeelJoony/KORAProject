#include "Camera/KRCameraMode_ThirdPerson.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Canvas.h"
#include "DrawDebugHelpers.h"
#include "Camera/KRCameraAssistInterface.h"

UKRCameraMode_ThirdPerson::UKRCameraMode_ThirdPerson()
{
	TargetOffsetCurve = nullptr;

	PenetrationAvoidanceFeelers.Add(FKRPenetrationAvoidanceFeeler(FRotator::ZeroRotator, 1.0f, 1.0f, 14.0f, 0));
	
	PenetrationAvoidanceFeelers.Add(FKRPenetrationAvoidanceFeeler(FRotator(0.0f, 15.0f, 0.0f), 0.75f, 0.75f, 0.0f, 3));
	PenetrationAvoidanceFeelers.Add(FKRPenetrationAvoidanceFeeler(FRotator(0.0f, -15.0f, 0.0f), 0.75f, 0.75f, 0.0f, 3));
	PenetrationAvoidanceFeelers.Add(FKRPenetrationAvoidanceFeeler(FRotator(15.0f, 0.0f, 0.0f), 0.75f, 0.75f, 0.0f, 5));
	PenetrationAvoidanceFeelers.Add(FKRPenetrationAvoidanceFeeler(FRotator(-15.0f, 0.0f, 0.0f), 0.50f, 0.50f, 0.0f, 5));
}

void UKRCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation(); 
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
	
	if (TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}
	
	UpdatePreventPenetration(DeltaTime);
}

void UKRCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)
{
	if (!bPreventPenetration) return;

	AActor* TargetActor = GetTargetActor();
	if (!TargetActor) return;

	APawn* TargetPawn = Cast<APawn>(TargetActor);
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
	
	IKRCameraAssistInterface* TargetControllerAssist = Cast<IKRCameraAssistInterface>(TargetController);
	IKRCameraAssistInterface* TargetActorAssist = Cast<IKRCameraAssistInterface>(TargetActor);
	
	TOptional<AActor*> OptionalPPTarget = TargetActorAssist ? TargetActorAssist->GetCameraPreventPenetrationTarget() : TOptional<AActor*>();
	AActor* PPActor = OptionalPPTarget.IsSet() ? OptionalPPTarget.GetValue() : TargetActor;
	IKRCameraAssistInterface* PPActorAssist = OptionalPPTarget.IsSet() ? Cast<IKRCameraAssistInterface>(PPActor) : nullptr;
	
	FVector SafeLocation = PPActor->GetActorLocation();
	
	FVector ClosestPointOnLineToCapsuleCenter;
	FMath::PointDistToLine(SafeLocation, View.Rotation.Vector(), View.Location, ClosestPointOnLineToCapsuleCenter);
	
	const float PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
	float const MaxHalfHeight = PPActor->GetSimpleCollisionHalfHeight() - PushInDistance;
	
	SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

	bool const bSingleRayPenetrationCheck = !bDoPredictiveAvoidance;
	PreventCameraPenetration(*PPActor, SafeLocation, View.Location, DeltaTime, AimLineToDesiredPosBlockedPct, bSingleRayPenetrationCheck);
	
	if (AimLineToDesiredPosBlockedPct < ReportPenetrationPercent)
	{
		if (TargetControllerAssist) TargetControllerAssist->OnCameraPenetratingTarget();
		if (TargetActorAssist) TargetActorAssist->OnCameraPenetratingTarget();
		if (PPActorAssist) PPActorAssist->OnCameraPenetratingTarget();
	}
}

void UKRCameraMode_ThirdPerson::PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc,
	FVector& CameraLoc, float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly)
{
	float HardBlockedPct = DistBlockedPct;
	float SoftBlockedPct = DistBlockedPct;

	FVector BaseRay = CameraLoc - SafeLoc;
	FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
	FVector BaseRayLocalUp, BaseRayLocalFwd, BaseRayLocalRight;
	BaseRayMatrix.GetScaledAxes(BaseRayLocalFwd, BaseRayLocalRight, BaseRayLocalUp);

	float DistBlockedPctThisFrame = 1.f;

	const int32 NumRaysToShoot = bSingleRayOnly ? FMath::Min(1, PenetrationAvoidanceFeelers.Num()) : PenetrationAvoidanceFeelers.Num();
	
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(CameraPen), false, nullptr);
	SphereParams.AddIgnoredActor(&ViewTarget);
	
	if (const IKRCameraAssistInterface* AssistInterface = Cast<IKRCameraAssistInterface>(&ViewTarget))
	{
		TArray<const AActor*> IgnoredActors;
		AssistInterface->GetIgnoredActorsForCameraPenetration(IgnoredActors);
		for (const AActor* Ignored : IgnoredActors)
		{
			SphereParams.AddIgnoredActor(Ignored);
		}
	}

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);
	UWorld* World = GetWorld();

	for (int32 RayIdx = 0; RayIdx < NumRaysToShoot; ++RayIdx)
	{
		FKRPenetrationAvoidanceFeeler& Feeler = PenetrationAvoidanceFeelers[RayIdx];
		if (Feeler.FramesUntilNextTrace <= 0)
		{
			FVector RayTarget;
			{
				FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);
				RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);
				RayTarget = SafeLoc + RotatedRay;
			}

			SphereShape.Sphere.Radius = Feeler.Extent;
			ECollisionChannel TraceChannel = ECC_Camera; 

			FHitResult Hit;
			const bool bHit = World->SweepSingleByChannel(Hit, SafeLoc, RayTarget, FQuat::Identity, TraceChannel, SphereShape, SphereParams);

			Feeler.FramesUntilNextTrace = Feeler.TraceInterval;

			if (bHit && Hit.GetActor())
			{
				float const Weight = Cast<APawn>(Hit.GetActor()) ? Feeler.PawnWeight : Feeler.WorldWeight;
				float NewBlockPct = Hit.Time;
				NewBlockPct += (1.f - NewBlockPct) * (1.f - Weight);

				NewBlockPct = ((Hit.Location - SafeLoc).Size() - CollisionPushOutDistance) / (RayTarget - SafeLoc).Size();
				DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);

				Feeler.FramesUntilNextTrace = 0;
			}

			if (RayIdx == 0)
			{
				HardBlockedPct = DistBlockedPctThisFrame;
			}
			else
			{
				SoftBlockedPct = DistBlockedPctThisFrame;
			}
		}
		else
		{
			--Feeler.FramesUntilNextTrace;
		}
	}

	// 보간 처리
	if (DistBlockedPct < DistBlockedPctThisFrame)
	{
		if (PenetrationBlendOutTime > DeltaTime)
		{
			DistBlockedPct = DistBlockedPct + DeltaTime / PenetrationBlendOutTime * (DistBlockedPctThisFrame - DistBlockedPct);
		}
		else
		{
			DistBlockedPct = DistBlockedPctThisFrame;
		}
	}
	else
	{
		if (DistBlockedPct > HardBlockedPct)
		{
			DistBlockedPct = HardBlockedPct;
		}
		else if (DistBlockedPct > SoftBlockedPct)
		{
			if (PenetrationBlendInTime > DeltaTime)
			{
				DistBlockedPct = DistBlockedPct - DeltaTime / PenetrationBlendInTime * (DistBlockedPct - SoftBlockedPct);
			}
			else
			{
				DistBlockedPct = SoftBlockedPct;
			}
		}
	}

	DistBlockedPct = FMath::Clamp<float>(DistBlockedPct, 0.f, 1.f);
	
	if (DistBlockedPct < (1.f - KINDA_SMALL_NUMBER))
	{
		CameraLoc = SafeLoc + (CameraLoc - SafeLoc) * DistBlockedPct;
	}
}

void UKRCameraMode_ThirdPerson::DrawDebug(UCanvas* Canvas) const
{
	Super::DrawDebug(Canvas);
}
