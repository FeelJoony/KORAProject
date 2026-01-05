#include "Camera/KRCameraMode_Burst.h"
#include "Components/KRCameraComponent.h"
#include "GameFramework/Character.h"

UKRCameraMode_Burst::UKRCameraMode_Burst()
{
	// 기본 카메라 위치 설정
	BackwardDistance = 500.0f;
	HeightOffset = 200.0f;
	SideOffset = 100.0f;
	LookAtHeightOffset = 50.0f;
	CameraInterpSpeed = 10.0f;

	// FOV 설정 - 약간 좁은 FOV로 집중감 연출
	SkillFOV = 75.0f;
	FieldOfView = SkillFOV;
	FOVInterpSpeed = 5.0f;

	// 블렌딩 설정 - 즉시 전환
	BlendTime = 0.15f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
	BlendFunction = EKRCameraModeBlendFunction::EaseOut;

	// 피치 제한
	ViewPitchMin = -60.0f;
	ViewPitchMax = 30.0f;

	// 충돌 방지 비활성화 (스킬 연출 우선)
	bPreventPenetration = false;
}

void UKRCameraMode_Burst::OnActivation()
{
	Super::OnActivation();

	// 현재 카메라 위치에서 시작
	if (UKRCameraComponent* CameraComp = GetKRCameraComponent())
	{
		CurrentCameraLocation = CameraComp->GetComponentLocation();
		CurrentFOV = FieldOfView;
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CameraMode_Burst] Activated"));
#endif
}

void UKRCameraMode_Burst::OnDeactivation()
{
	Super::OnDeactivation();
	ClearBurstCamera();

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CameraMode_Burst] Deactivated"));
#endif
}

void UKRCameraMode_Burst::SetupBurstCamera(const FVector& PlayerLocation, const FVector& PlayerForward)
{
	PlayerStartLocation = PlayerLocation;
	PlayerStartForward = PlayerForward.GetSafeNormal2D();
	bCameraSetup = true;

	// 고정 카메라 위치 계산
	FixedCameraLocation = CalculateTargetCameraLocation();
	CurrentCameraLocation = FixedCameraLocation;

	// 고정 카메라 회전 계산 (시작 위치 기준 LookAt)
	FVector StartLookAtLocation = PlayerStartLocation + FVector(0.0f, 0.0f, LookAtHeightOffset);
	FVector LookDirection = (StartLookAtLocation - FixedCameraLocation).GetSafeNormal();
	FixedCameraRotation = LookDirection.Rotation();
	FixedCameraRotation.Pitch = FMath::Clamp(FixedCameraRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// 블렌딩 즉시 완료
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CameraMode_Burst] Setup - PlayerLocation: %s, CameraLocation: %s, TrackPlayer: %s"),
		*PlayerStartLocation.ToString(), *FixedCameraLocation.ToString(), bTrackPlayer ? TEXT("true") : TEXT("false"));
#endif
}

void UKRCameraMode_Burst::ClearBurstCamera()
{
	bCameraSetup = false;
	PlayerStartLocation = FVector::ZeroVector;
	PlayerStartForward = FVector::ForwardVector;
	FixedCameraLocation = FVector::ZeroVector;
	FixedCameraRotation = FRotator::ZeroRotator;
	PlayerActor.Reset();
}

void UKRCameraMode_Burst::UpdateView(float DeltaTime)
{
	// 카메라가 설정되지 않았으면 부모 클래스 사용
	if (!bCameraSetup)
	{
		Super::UpdateView(DeltaTime);
		return;
	}

	// 카메라 위치 - 고정 위치 사용
	FVector TargetCameraLocation = FixedCameraLocation;

	// 부드러운 카메라 이동 (선택적)
	CurrentCameraLocation = FMath::VInterpTo(
		CurrentCameraLocation,
		TargetCameraLocation,
		DeltaTime,
		CameraInterpSpeed
	);

	// FOV 보간
	CurrentFOV = FMath::FInterpTo(
		CurrentFOV,
		SkillFOV,
		DeltaTime,
		FOVInterpSpeed
	);

	// 카메라 회전 결정
	FRotator FinalRotation;

	if (bTrackPlayer)
	{
		// 플레이어 추적 모드: 현재 플레이어 위치를 바라봄
		FVector LookAtLocation = CalculateLookAtLocation();
		FVector LookDirection = (LookAtLocation - CurrentCameraLocation).GetSafeNormal();
		FinalRotation = LookDirection.Rotation();
		FinalRotation.Pitch = FMath::Clamp(FinalRotation.Pitch, ViewPitchMin, ViewPitchMax);
	}
	else
	{
		// 고정 모드: 시작 시점의 회전 유지 (RootMotion 스킬용)
		FinalRotation = FixedCameraRotation;
	}

	// 뷰 설정
	View.Location = CurrentCameraLocation;
	View.Rotation = FinalRotation;
	View.ControlRotation = FinalRotation;
	View.FieldOfView = CurrentFOV;
}

FVector UKRCameraMode_Burst::GetPivotLocation() const
{
	if (bCameraSetup)
	{
		return PlayerStartLocation;
	}

	return Super::GetPivotLocation();
}

FRotator UKRCameraMode_Burst::GetPivotRotation() const
{
	if (bCameraSetup)
	{
		FVector LookAtLocation = CalculateLookAtLocation();
		FVector LookDirection = (LookAtLocation - CurrentCameraLocation).GetSafeNormal();
		return LookDirection.Rotation();
	}

	return Super::GetPivotRotation();
}

FVector UKRCameraMode_Burst::CalculateTargetCameraLocation() const
{
	if (!bCameraSetup)
	{
		return FVector::ZeroVector;
	}

	// 플레이어 뒤쪽 방향
	FVector BackwardDirection = -PlayerStartForward;

	// 우측 방향 (측면 오프셋용)
	FVector RightDirection = FVector::CrossProduct(FVector::UpVector, PlayerStartForward).GetSafeNormal();

	// 카메라 위치 계산
	// = 플레이어 시작 위치 + 뒤쪽 오프셋 + 높이 오프셋 + 측면 오프셋
	FVector CameraLocation = PlayerStartLocation
		+ (BackwardDirection * BackwardDistance)
		+ FVector(0.0f, 0.0f, HeightOffset)
		+ (RightDirection * SideOffset);

	return CameraLocation;
}

FVector UKRCameraMode_Burst::CalculateLookAtLocation() const
{
	// 플레이어 액터가 있으면 현재 위치 추적
	if (PlayerActor.IsValid())
	{
		return PlayerActor->GetActorLocation() + FVector(0.0f, 0.0f, LookAtHeightOffset);
	}

	// 없으면 시작 위치 사용
	return PlayerStartLocation + FVector(0.0f, 0.0f, LookAtHeightOffset);
}
