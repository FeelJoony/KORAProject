#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "KRCameraMode.generated.h"

class UKRCameraComponent;
class AActor;
class UCanvas;

UENUM(BlueprintType)
enum class EKRCameraModeBlendFunction : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT UMETA(Hidden)
};

struct FKRCameraModeView
{
	FKRCameraModeView();

	void Blend(const FKRCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

UCLASS(Abstract, NotBlueprintable)
class KORAPROJECT_API UKRCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UKRCameraMode();

	UKRCameraComponent* GetKRCameraComponent() const;
	virtual UWorld* GetWorld() const override;
	AActor* GetTargetActor() const;

	const FKRCameraModeView& GetCameraModeView() const { return View; }

	virtual void OnActivation() {};
	virtual void OnDeactivation() {};

	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

	FGameplayTag GetCameraTypeTag() const { return CameraTypeTag; }

	virtual void DrawDebug(UCanvas* Canvas) const;

protected:
	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	FGameplayTag CameraTypeTag;

	FKRCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "view", meta = (ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "view", meta = (ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "view", meta = (ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	EKRCameraModeBlendFunction BlendFunction;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;
	
	float BlendAlpha;
	float BlendWeight;

	// 1프레임 동안 보간을 건너뛰고 싶을 때 사용
	UPROPERTY(Transient)
	uint32 bResetInterpolation:1;
};


UCLASS()
class KORAPROJECT_API UKRCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UKRCameraModeStack();

	void ActivateStack();
	void DeactivateStack();
	bool IsStackActivate() const {return bIsActive; }

	// 새로운 모드를 스택에 추가
	void PushCameraMode(TSubclassOf<UKRCameraMode> CameraModeClass);

	// 스택 평가 및 최종 뷰 결과 생성
	bool EvaluateStack(float DeltaTime, FKRCameraModeView& OutCameraModeView);

	// 디버깅 정보 제공
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
	void DrawDebug(UCanvas* Canvas) const;

protected:
	UPROPERTY()
	TArray<TObjectPtr<UKRCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<UKRCameraMode>> CameraModeStack;

	bool bIsActive;
	
	UKRCameraMode* GetCameraModeInstance(TSubclassOf<UKRCameraMode> CameraModeClass);
	void UpdateStack(float DeltaTime);
	void BlendStack(FKRCameraModeView& OutCameraModeView) const;
	
};
