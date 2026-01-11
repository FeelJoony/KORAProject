// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "KRBaseGameState.generated.h"

/**
 * 
 */
UCLASS()
class KORAPROJECT_API AKRBaseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AKRBaseGameState();

	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	TObjectPtr<class UKRExperienceManagerComponent> ExperienceManagerComponent;
};
