#pragma once

#include "CoreMinimal.h"

/*테스트용 임시*/
#include "GameplayAbilitySpecHandle.h"

#include "Characters/KRBaseCharacter.h"
#include "KRHeroCharacter.generated.h"


class UDataAsset_InputConfig;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;


UCLASS()
class KORAPROJECT_API AKRHeroCharacter : public AKRBaseCharacter
{
	GENERATED_BODY()
	
public:
	AKRHeroCharacter();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);

	/*테스트용 임시 바인딩*/
	UPROPERTY()
	FGameplayAbilitySpecHandle CurrentSprintHandle;
	void Input_Sprint_Started(const FInputActionValue& Value);
	void Input_Sprint_Released(const FInputActionValue& Value);
};
