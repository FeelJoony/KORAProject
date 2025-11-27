#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "Camera/KRCameraAssistInterface.h"
#include "KRHeroCharacter.generated.h"


class UKRHeroComponent;
class UKRCameraComponent;

UCLASS()
class KORAPROJECT_API AKRHeroCharacter : public AKRBaseCharacter, public IKRCameraAssistInterface
{
	GENERATED_BODY()
	
public:
	AKRHeroCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Camera")
	TObjectPtr<UKRCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Component")
	TObjectPtr<UKRHeroComponent> HeroComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UActorComponent> CachedOutlinerComponent;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetIgnoredActorsForCameraPenetration(TArray<const AActor*>& OutActorsAllowPenetration) const override;
	virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const override;
};

