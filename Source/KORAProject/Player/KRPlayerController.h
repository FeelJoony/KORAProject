#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "KRPlayerController.generated.h"

class AKRPlayerState;
class UKRAbilitySystemComponent;
class AInteractableActorBase;
class AKRQuestItemPickup;

UCLASS()
class KORAPROJECT_API AKRPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
	AKRPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void OnUnPossess() override;
	
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	UFUNCTION(BlueprintCallable, Category = "KR|PlayerController")
	AKRPlayerState* GetKRPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "KR|PlayerController")
	UKRAbilitySystemComponent* GetKRAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "KR|Interaction")
	void SetCurrentInteractableActor(AInteractableActorBase* InActor);

	UFUNCTION(BlueprintPure, Category = "KR|Interaction")
	AInteractableActorBase* GetCurrentInteractableActor() const;

private:
	UPROPERTY()
	TWeakObjectPtr<AInteractableActorBase> CurrentInteractableActor;
};
