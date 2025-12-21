// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "KRQuestAcceptTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class KORAPROJECT_API AKRQuestAcceptTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AKRQuestAcceptTriggerBox();
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(EditInstanceOnly, Category = "Quest|Index")
	int32 QuestIndex;
	
};
