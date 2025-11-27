#include "Camera/KRCameraMode.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Canvas.h"
#include "GameFramework/Character.h"
#include "Components/KRCameraComponent.h"
#include "Camera/KRPlayerCameraManager.h"

//////////////////////////////////////////////////////////////////////////
// FKRCameraModeView
//////////////////////////////////////////////////////////////////////////
FKRCameraModeView::FKRCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(KORA_CAMERA_DEFAULT_FOV)
{
}

void FKRCameraModeView::Blend(const FKRCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <=0.f) return;
	if (OtherWeight >= 1.f)
	{
		*this = Other;
		return;
	}

	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	const FRotator DeltaRot = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRot);

	const FRotator DeltaControlRot = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRot);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

//////////////////////////////////////////////////////////////////////////
// UKRCameraMode
//////////////////////////////////////////////////////////////////////////
UKRCameraMode::UKRCameraMode()
{
	FieldOfView = KORA_CAMERA_DEFAULT_FOV;
	ViewPitchMin = KORA_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = KORA_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.5;
	BlendFunction = EKRCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.f;
	BlendAlpha = 1.f;
	BlendWeight = 1.f;
}

UKRCameraComponent* UKRCameraMode::GetKRCameraComponent() const
{
	return CastChecked<UKRCameraComponent>(GetOuter());
}

UWorld* UKRCameraMode::GetWorld() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter()->GetWorld();
}

AActor* UKRCameraMode::GetTargetActor() const
{
	const UKRCameraComponent* KRCameraComponent = GetKRCameraComponent();

	return KRCameraComponent->GetTargetActor();
}

void UKRCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlending(DeltaTime);
}

void UKRCameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.f, 1.f);

	const float InvExponent = (BlendExponent > 0.f) ? (1.f / BlendExponent) : 1.f;

	switch (BlendFunction)
	{
	case EKRCameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;
		
	case EKRCameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.f, 1.f, BlendWeight, InvExponent);
		break;

	case EKRCameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.f, 1.f, BlendWeight, InvExponent);
		break;
	case EKRCameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.f, 1.f, BlendWeight, InvExponent);
		break;

	default:
		checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

void UKRCameraMode::DrawDebug(UCanvas* Canvas) const
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetDrawColor(FColor::White);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("      KRCameraMode: %s (%f)"), *GetName(), BlendWeight));
}

FVector UKRCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UKRCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetActorRotation();
	}

	return TargetActor->GetActorRotation();
}

void UKRCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}

void UKRCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
		BlendAlpha = FMath::Min(BlendAlpha, 1.f);
	}
	else
	{
		BlendAlpha = 1.f;
	}

	const float Exponent = (BlendExponent > 0.f) ? BlendExponent : 1.f;

	switch (BlendFunction)
	{
	case EKRCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;

	case EKRCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EKRCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	case EKRCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;

	default:
		checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// UKRCameraModeStack
//////////////////////////////////////////////////////////////////////////
UKRCameraModeStack::UKRCameraModeStack()
{
	bIsActive = true;
}

void UKRCameraModeStack::ActivateStack()
{
	if (!bIsActive)
	{
		bIsActive = true;

		for (UKRCameraMode* CameraMode : CameraModeStack)
		{
			check(CameraMode);
			CameraMode->OnActivation();
		}
	}
}

void UKRCameraModeStack::DeactivateStack()
{
	if (bIsActive)
	{
		bIsActive = false;

		for (UKRCameraMode* CameraMode : CameraModeStack)
		{
			check(CameraMode);
			CameraMode->OnDeactivation();
		}
	}
}

void UKRCameraModeStack::PushCameraMode(TSubclassOf<UKRCameraMode> CameraModeClass)
{
	if (!CameraModeClass) return;

	UKRCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();

	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		return;
	}

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.f;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			ExistingStackContribution *= CameraMode->GetBlendWeight();
			break;
		}
		else
		{
			ExistingStackContribution *= (1.f - CameraModeStack[StackIndex]->GetBlendWeight());
		}
	}

	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		ExistingStackContribution = 0.f;
	}

	const bool bShouldBlend = ((CameraMode->GetBlendTime() > 0.f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.f);

	CameraMode->SetBlendWeight(BlendWeight);

	CameraModeStack.Insert(CameraMode, 0);
	CameraModeStack.Last()->SetBlendWeight(1.f);

	if (ExistingStackIndex == INDEX_NONE)
	{
		CameraMode->OnActivation();
	}
}

bool UKRCameraModeStack::EvaluateStack(float DeltaTime, FKRCameraModeView& OutCameraModeView)
{
	if (!bIsActive) return false;

	UpdateStack(DeltaTime);
	BlendStack(OutCameraModeView);

	return true;
}

void UKRCameraModeStack::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const
{
	if (CameraModeStack.Num() == 0)
	{
		OutWeightOfTopLayer = 1.f;
		OutTagOfTopLayer = FGameplayTag();
	}
	else
	{
		UKRCameraMode* TopEntry = CameraModeStack.Last();
		check(TopEntry);
		OutWeightOfTopLayer = TopEntry->GetBlendWeight();
		OutTagOfTopLayer = TopEntry->GetCameraTypeTag();
	}
}

void UKRCameraModeStack::DrawDebug(UCanvas* Canvas) const
{
	check(Canvas);
	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString(TEXT("   --- Camera Modes (Begin) ---")));

	for (const UKRCameraMode* CameraMode : CameraModeStack)
	{
		check(CameraMode);
		CameraMode->DrawDebug(Canvas);
	}

	DisplayDebugManager.SetDrawColor(FColor::Green);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("   --- Camera Modes (End) ---")));
}

UKRCameraMode* UKRCameraModeStack::GetCameraModeInstance(TSubclassOf<UKRCameraMode> CameraModeClass)
{
	check(CameraModeClass);

	for (UKRCameraMode* CameraMode : CameraModeInstances)
	{
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	UKRCameraMode* NewCameraMode = NewObject<UKRCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);
	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void UKRCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0) return;

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UKRCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		if (CameraMode->GetBlendWeight() >= 1.f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}
	
	if(RemoveCount > 0)
	{
		for (int32 StackIndex = RemoveIndex; StackIndex < StackSize; ++StackIndex)
		{
			UKRCameraMode* CameraMode = CameraModeStack[StackIndex];
			check(CameraMode);
			CameraMode->OnDeactivation();
		}
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UKRCameraModeStack::BlendStack(FKRCameraModeView& OutCameraModeView) const
{

	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0) return;

	const UKRCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->GetCameraModeView();

	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);
		OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
	}
}
