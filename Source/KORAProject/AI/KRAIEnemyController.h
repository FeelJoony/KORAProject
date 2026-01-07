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
	// Sets default values for this actor's properties
	AKRAIEnemyController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
