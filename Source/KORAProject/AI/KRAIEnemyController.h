// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "KRAIEnemyController.generated.h"

struct FAIStimulus;

UCLASS()
class KORAPROJECT_API AKRAIEnemyController : public AModularAIController
{
	GENERATED_BODY()

public:
	AKRAIEnemyController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StateTree)
	TObjectPtr<class UStateTreeAIComponent> StateTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIPerception)
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TWeakObjectPtr<AActor> TargetActor;

protected:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UPROPERTY()
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

private:
	bool IsPlayerCharacter(AActor* InActor);
	APawn* ResolveToPawn(AActor* InActor) const;
};
