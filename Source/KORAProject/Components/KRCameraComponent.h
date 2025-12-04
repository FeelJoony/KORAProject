#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameplayTagContainer.h"
#include "KRCameraComponent.generated.h"

class UKRCameraMode;
class UKRCameraModeStack;
struct FKRCameraModeView;

DECLARE_DELEGATE_RetVal(TSubclassOf<UKRCameraMode>, FKRCameraModeDelegate);

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class KORAPROJECT_API UKRCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UKRCameraComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "KR|Camera")
	static UKRCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UKRCameraComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = "KR|Camera")
	void PushCameraMode(TSubclassOf<UKRCameraMode> CameraModeClass);
	
	UFUNCTION(BlueprintCallable, Category = "KR|Camera")
	void RemoveCameraMode(TSubclassOf<UKRCameraMode> CameraModeClass);
	
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }
	
	FKRCameraModeDelegate DetermineCameraModeDelegate;

	virtual void DrawDebug(UCanvas* Canvas) const;
	
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:
	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

	UPROPERTY()
	TObjectPtr<UKRCameraModeStack> CameraModeStack;

	float FieldOfViewOffset;
};
