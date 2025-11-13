#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractableTarget.h"
#include "InteractableItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class KORAPROJECT_API AInteractableItem : public AActor, public IInteractableTarget
{
	GENERATED_BODY()

public:
	AInteractableItem();

	// IInteractableTarget interface
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;
	// End IInteractableTarget interface

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionSubText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TSubclassOf<UGameplayAbility> InteractionGA;

};
