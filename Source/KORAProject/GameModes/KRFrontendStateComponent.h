#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "KRFrontendStateComponent.generated.h"

class UKRUserFacingExperience;
class UKRExperienceDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFrontendReady);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class KORAPROJECT_API UKRFrontendStateComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UKRFrontendStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UserFacingExperience for new game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Frontend")
	TSoftObjectPtr<UKRUserFacingExperience> DefaultGameplayExperience;

	// Main menu route name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Frontend")
	FName MainMenuRouteName = TEXT("MainMenu");

	// Called when frontend is ready (Blueprint overridable)
	UPROPERTY(BlueprintAssignable, Category = "Frontend|Events")
	FOnFrontendReady OnFrontendReady;

	// Show main menu widget
	UFUNCTION(BlueprintCallable, Category = "Frontend")
	void ShowMainMenu();

	// Travel to gameplay (new game)
	UFUNCTION(BlueprintCallable, Category = "Frontend")
	void TravelToGameplay();

	// Travel to continue (load save and travel)
	UFUNCTION(BlueprintCallable, Category = "Frontend")
	void TravelToContinue();

	// Check if save data exists
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Frontend")
	bool HasSaveData() const;

protected:
	virtual void BeginPlay() override;

	// Blueprint native event - called when frontend is ready
	UFUNCTION(BlueprintNativeEvent, Category = "Frontend")
	void OnFrontendReadyEvent();
	virtual void OnFrontendReadyEvent_Implementation();

private:
	void OnExperienceLoaded(const UKRExperienceDefinition* Experience);
	void TryShowMainMenu();

	bool bIsExperienceLoaded = false;
	bool bHasShownMainMenu = false;
};
