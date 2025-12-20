#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "KRHeroComponent.generated.h"

class UKRCameraMode;
class UInputComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;
class UGameFrameworkComponentManager;
struct FActorInitStateChangedParams;


UCLASS()
class KORAPROJECT_API UKRHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UKRHeroComponent(const FObjectInitializer& ObjectInitializer);

	static const FName NAME_ActorFeatureName;

	static const FName NAME_BindInputsNow;

	//~ Begin IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface

	UFUNCTION(BlueprintCallable, Category = "KR|Input")
	FVector2D GetLastLookInput() const { return LastLookInput; }
	
	UFUNCTION(BlueprintCallable, Category = "KR|Input")
	FVector2D GetLastMoveInput() const { return LastMoveInput; }
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);

	void Input_AbilityInputPressed(FGameplayTag InputTag);
	void Input_AbilityInputReleased(FGameplayTag InputTag);

	TSubclassOf<UKRCameraMode> DetermineCameraMode() const;

	UPROPERTY()
	TSubclassOf<UKRCameraMode> AbilityCameraMode;

	FVector2D LastLookInput = FVector2D::ZeroVector;
    FVector2D LastMoveInput = FVector2D::ZeroVector;

private:
	bool bReadyToBindInputs;
};
