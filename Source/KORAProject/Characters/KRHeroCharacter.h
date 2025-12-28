#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "Camera/KRCameraAssistInterface.h"
#include "KRHeroCharacter.generated.h"

class UMotionWarpingComponent;
class UKRHeroComponent;
class UKRCameraComponent;
class UKRStaminaComponent;
class UKRCoreDriveComponent;
class UKRGuardRegainComponent;

UCLASS()
class KORAPROJECT_API AKRHeroCharacter : public AKRBaseCharacter, public IKRCameraAssistInterface
{
	GENERATED_BODY()
	
public:
	AKRHeroCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category = "KR|Movement")
	void SetMovementMode_Strafe(bool bStrafe);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Camera")
	TObjectPtr<UKRCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Component")
	TObjectPtr<UKRHeroComponent> HeroComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Component")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Component")
	TObjectPtr<UKRStaminaComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Component")
	TObjectPtr<UKRCoreDriveComponent> CoreDriveComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Component")
	TObjectPtr<UKRGuardRegainComponent> GuardRegainComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UActorComponent> CachedOutlinerComponent;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetIgnoredActorsForCameraPenetration(TArray<const AActor*>& OutActorsAllowPenetration) const override;
	virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const override;
};

