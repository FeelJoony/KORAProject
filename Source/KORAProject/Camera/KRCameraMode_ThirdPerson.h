#pragma once

#include "CoreMinimal.h"
#include "Camera/KRCameraMode.h"
#include "Curves/CurveVector.h"
#include "KRCameraMode_ThirdPerson.generated.h"

USTRUCT(BlueprintType)
struct FKRPenetrationAvoidanceFeeler
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Penetration")
	FRotator AdjustmentRot = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Penetration")
	float WorldWeight = 0.f;

	UPROPERTY(EditAnywhere, Category = "Penetration")
	float PawnWeight = 0.f;

	UPROPERTY(EditAnywhere, Category = "Penetration")
	float Extent = 0.f;

	UPROPERTY(EditAnywhere, Category = "Penetration")
	int32 TraceInterval = 0;

	UPROPERTY(Transient)
	int32 FramesUntilNextTrace = 0;

	FKRPenetrationAvoidanceFeeler() {}
	FKRPenetrationAvoidanceFeeler(const FRotator& InRot, float InWorldWeight, float InPawnWeight, float InExtent, int32 InInterval = 0)
		: AdjustmentRot(InRot), WorldWeight(InWorldWeight), PawnWeight(InPawnWeight), Extent(InExtent), TraceInterval(InInterval) {}
};

UCLASS(Abstract, Blueprintable)
class KORAPROJECT_API UKRCameraMode_ThirdPerson : public UKRCameraMode
{
	GENERATED_BODY()

public:
	UKRCameraMode_ThirdPerson();

protected:
	virtual void UpdateView(float DeltaTime) override;
	
	void UpdatePreventPenetration(float DeltaTime);
	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc, float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);

	virtual void DrawDebug(UCanvas* Canvas) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float PenetrationBlendInTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float PenetrationBlendOutTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bPreventPenetration = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bDoPredictiveAvoidance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float CollisionPushOutDistance = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float ReportPenetrationPercent = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TArray<FKRPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPct;
};
