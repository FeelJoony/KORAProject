// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractableTarget.h"
#include "InteractableItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * Example interactable item that can be picked up or interacted with
 */
UCLASS()
class KORAPROJECT_API AInteractableItem : public AActor, public IInteractableTarget
{
	GENERATED_BODY()

public:
	AInteractableItem();

	// IInteractableTarget interface
	virtual void GatherInteractionOptions_Implementation(const FInteractionQuery& InteractQuery, TArray<FInteractionOption>& Options) override;
	virtual void OnInteractionTriggered_Implementation(AActor* InteractingActor) override;
	virtual FVector GetInteractionIndicatorLocation_Implementation() const override;
	virtual bool CanInteract_Implementation(const FInteractionQuery& InteractQuery) const override;
	// End IInteractableTarget interface

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionSubText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteract;

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction") //BP에서 구현 왜지 굳이?.
	void OnInteracted(AActor* InteractingActor);
};
