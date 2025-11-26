#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "KRHeroCharacter.generated.h"


class UKRHeroComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class KORAPROJECT_API AKRHeroCharacter : public AKRBaseCharacter
{
	GENERATED_BODY()
	
public:
	AKRHeroCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UKRHeroComponent> HeroComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UActorComponent> CachedOutlinerComponent;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

};

