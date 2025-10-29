#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "KRHeroCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;

UCLASS()
class KORAPROJECT_API AKRHeroCharacter : public AKRBaseCharacter
{
	GENERATED_BODY()
	
public:
	AKRHeroCharacter();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;
};
