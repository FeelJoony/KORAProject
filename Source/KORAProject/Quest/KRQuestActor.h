// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRQuestActor.generated.h"

UCLASS()
class KORAPROJECT_API AKRQuestActor : public AActor
{
	GENERATED_BODY()

public:
	AKRQuestActor();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<class UStateTreeComponent> QuestStateTreeComp;

	void SetQuestInstance(class UKRQuestInstance* NewQuestInstance);
	class UKRQuestInstance* GetQuestInstance() const;
	
private:
	UPROPERTY()
	TObjectPtr<class UKRQuestInstance> QuestInstance;

	float CheckTime = 5.f;
	float ElapsedTime = 0.f;
};

