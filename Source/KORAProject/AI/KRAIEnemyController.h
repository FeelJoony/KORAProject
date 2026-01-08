// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "KRAIEnemyController.generated.h"

UCLASS()
class KORAPROJECT_API AKRAIEnemyController : public AModularAIController
{
	GENERATED_BODY()

public:
	AKRAIEnemyController();

	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StateTree)
	TObjectPtr<class UStateTreeAIComponent> StateTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AIPerception)
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComponent;
};
